/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：将gif错误功能
版本号：1.0.0
开发时期：2003-03-06
作者：孔
修改记录：
******************************************************/

#include <ewindows.h>
#include "gif_lib.h"

int g_GifError = 0;

/*****************************************************************************
* Return the last GIF error (0 if none) and reset the error.		     *
*****************************************************************************/
int GifLastError(void)
{
    int i = g_GifError;

    g_GifError = 0;

    return i;
}

/*****************************************************************************
* Print the last GIF error to stderr.					     *
*****************************************************************************/
void PrintGifError(void)
{
    char *Err;

    switch(g_GifError) {
	case D_GIF_ERR_OPEN_FAILED:
	    Err = "Failed to open given file";
	    break;
	case D_GIF_ERR_READ_FAILED:
	    Err = "Failed to Read from given file";
	    break;
	case D_GIF_ERR_NOT_GIF_FILE:
	    Err = "Given file is NOT GIF file";
	    break;
	case D_GIF_ERR_NO_SCRN_DSCR:
	    Err = "No Screen Descriptor detected";
	    break;
	case D_GIF_ERR_NO_IMAG_DSCR:
	    Err = "No Image Descriptor detected";
	    break;
	case D_GIF_ERR_NO_COLOR_MAP:
	    Err = "Neither Global Nor Local color map";
	    break;
	case D_GIF_ERR_WRONG_RECORD:
	    Err = "Wrong record type detected";
	    break;
	case D_GIF_ERR_DATA_TOO_BIG:
	    Err = "#Pixels bigger than Width * Height";
	    break;
	case D_GIF_ERR_NOT_ENOUGH_MEM:
	    Err = "Fail to allocate required memory";
	    break;
	case D_GIF_ERR_CLOSE_FAILED:
	    Err = "Failed to close given file";
	    break;
	case D_GIF_ERR_NOT_READABLE:
	    Err = "Given file was not opened for read";
	    break;
	case D_GIF_ERR_IMAGE_DEFECT:
	    Err = "Image is defective, decoding aborted";
	    break;
	case D_GIF_ERR_EOF_TOO_SOON:
	    Err = "Image EOF detected, before image complete";
	    break;
	default:
	    Err = NULL;
	    break;
    }
    if (Err != NULL)
	    EdbgOutputDebugString("\nGIF-LIB error: %s.\n", Err);
    else
	    EdbgOutputDebugString("\nGIF-LIB undefined error %d.\n", g_GifError);
}
