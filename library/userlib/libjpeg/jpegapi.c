// ==========================================================
// JPEG Loader
// Based on code developed by The Independent JPEG Group
//
// Design and implementation by
// - Floris van den Berg (flvdberg@wxs.nl)
// - Jan L. Nauta (jln@magentammt.com)
// - Markus Loibl (markus.loibl@epost.de)
// - Karl-Heinz Bussian (khbussian@moss.de)
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

//#include "FreeImage.h"
#define JPEG_INTERNALS
//#include "Utilities.h"

#ifdef KINGMOS
	#include <eframe.h>
	#include <efile.h>
//	#include <esetjmp.h>
	#include <setjmp.h>	
	#include "jinclude.h"
	#include "jpeglib.h"
	#include <jpegapi.h>
#else
	#include <windows.h> 
	#include "jinclude.h"
	#include "jpeglib.h"
	#include <setjmp.h>
	#include <jpegapi.h>
	#define RETAILMSG
	#define WARNMSG
#endif
//#include "jpeglib.h"

//#include "jerror.h"



struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */

	jmp_buf setjmp_buffer;	/* for return to caller */
};

// ----------------------------------------------------------

typedef struct {
	struct jpeg_source_mgr pub;	/* public fields */

	HANDLE infile;		/* source stream */
	//FreeImageIO *m_io;

	JOCTET * buffer;		/* start of buffer */
	boolean start_of_file;	/* have we gotten any data yet? */
} my_source_mgr;

typedef struct {
	struct jpeg_destination_mgr pub;	/* public fields */

	HANDLE outfile;		/* destination stream */
	//FreeImageIO *m_io;

	JOCTET * buffer;		/* start of buffer */
} my_destination_mgr;

// ----------------------------------------------------------

typedef my_source_mgr * freeimage_src_ptr;
typedef my_destination_mgr * freeimage_dst_ptr;

// ==========================================================
//   JPEG LOAD/SAVE MODULE
// ==========================================================

#define INPUT_BUF_SIZE  4096	/* choose an efficiently fread'able size */
#define OUTPUT_BUF_SIZE 4096    /* choose an efficiently fwrite'able size */

#define FIF_JPEG 1
#define JERR_INPUT_EMPTY 2
#define WRN_JPEG_EOF 3
#define JERR_FILE_WRITE 4
#define JWRN_JPEG_EOF 5
typedef struct my_error_mgr * my_error_ptr;

#define PAL_BITFIELD 1
#define PAL_INDEX    2
// ----------------------------------------------------------
//   Constants + headers
// ----------------------------------------------------------

METHODDEF(void)
jpeg_error_exit (j_common_ptr cinfo) {
	(*cinfo->err->output_message)(cinfo);

	jpeg_destroy(cinfo);

	//throw FIF_JPEG;
	longjmp( ( (my_error_ptr)cinfo->err )->setjmp_buffer, FIF_JPEG );
}

METHODDEF(void)
jpeg_output_message (j_common_ptr cinfo) {
	char buffer[JMSG_LENGTH_MAX];

	(*cinfo->err->format_message)(cinfo, buffer);
	//FreeImage_OutputMessageProc(FIF_JPEG, buffer);

	RETAILMSG( 1, ( "%s.\r\n", buffer ) );
}

//===========================================================
/*
 * jdatasrc.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains decompression data source routines for the case of
 * reading JPEG data from a file (or any stdio stream).  While these routines
 * are sufficient for most applications, some will want to use a different
 * source manager.
 * IMPORTANT: we assume that fread() will correctly transcribe an array of
 * JOCTETs from 8-bit-wide elements on external storage.  If char is wider
 * than 8 bits on your machine, you may need to do some tweaking.
 */



// Expanded data source object for stdio input --------------


// ----------------------------------------------------------

METHODDEF(void)
init_source (j_decompress_ptr cinfo) {
	freeimage_src_ptr src = (freeimage_src_ptr) cinfo->src;

	/* We reset the empty-input-file flag for each image,
 	 * but we don't clear the input buffer.
	 * This is correct behavior for reading a series of images from one source.
	*/

	src->start_of_file = TRUE;
}

METHODDEF(void)
init_destination (j_compress_ptr cinfo) {
	freeimage_dst_ptr dest = (freeimage_dst_ptr) cinfo->dest;

	dest->buffer = (JOCTET *)
	  (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}

// ----------------------------------------------------------

/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * In typical applications, this should read fresh data into the buffer
 * (ignoring the current state of next_input_byte & bytes_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been reloaded.  It is not necessary to
 * fill the buffer entirely, only to obtain at least one more byte.
 *
 * There is no such thing as an EOF return.  If the end of the file has been
 * reached, the routine has a choice of throw() or inserting fake data into
 * the buffer.  In most cases, generating a warning message and inserting a
 * fake EOI marker is the best course of action --- this will allow the
 * decompressor to output however much of the image is there.  However,
 * the resulting error message is misleading if the real problem is an empty
 * input file, so we handle that case specially.
 *
 * In applications that need to be able to suspend compression due to input
 * not being available yet, a FALSE return indicates that no more data can be
 * obtained right now, but more may be forthcoming later.  In this situation,
 * the decompressor will return to its caller (with an indication of the
 * number of scanlines it has read, if any).  The application should resume
 * decompression after it has loaded more data into the input buffer.  Note
 * that there are substantial restrictions on the use of suspension --- see
 * the documentation.
 *
 * When suspending, the decompressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_input_byte & bytes_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point must be rescanned after resumption, so move it to
 * the front of the buffer rather than discarding it.
 */

METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo) {
	freeimage_src_ptr src = (freeimage_src_ptr) cinfo->src;

	//size_t nbytes = src->m_io->read_proc(src->buffer, 1, INPUT_BUF_SIZE, src->infile);
	size_t nbytes;
	
	ReadFile( src->infile, src->buffer, INPUT_BUF_SIZE, (LPDWORD)&nbytes, NULL );//src->infile);
	
	if (nbytes <= 0) {
		if (src->start_of_file)	/* Treat empty input file as fatal error */
		{
			//throw(cinfo, JERR_INPUT_EMPTY);
			longjmp( ( (my_error_ptr)cinfo->err )->setjmp_buffer, JERR_INPUT_EMPTY );
		}

		//WARNMSG( 1, ( "warn here.\r\n" ) );//cinfo, JWRN_JPEG_EOF);

		/* Insert a fake EOI marker */

		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;

		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}

METHODDEF(boolean)
empty_output_buffer (j_compress_ptr cinfo) {
	freeimage_dst_ptr dest = (freeimage_dst_ptr) cinfo->dest;
	DWORD dwWriteNum = 0;

	//if (dest->m_io->write_proc(dest->buffer, 1, OUTPUT_BUF_SIZE, dest->outfile) != OUTPUT_BUF_SIZE)
	//	throw(cinfo, JERR_FILE_WRITE);
    WriteFile( dest->outfile, dest->buffer, OUTPUT_BUF_SIZE, &dwWriteNum, NULL );
	if( dwWriteNum != OUTPUT_BUF_SIZE )
	{ //error
		longjmp( ( (my_error_ptr)cinfo->err )->setjmp_buffer, JERR_FILE_WRITE );
	}
	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

	return TRUE;
}

/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * Writers of suspendable-input applications must note that skip_input_data
 * is not granted the right to give a suspension return.  If the skip extends
 * beyond the data currently in the buffer, the buffer can be marked empty so
 * that the next read will cause a fill_input_buffer call that can suspend.
 * Arranging for additional bytes to be discarded before reloading the input
 * buffer is the application writer's problem.
 */

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes) {
	freeimage_src_ptr src = (freeimage_src_ptr) cinfo->src;

	/* Just a dumb implementation for now.  Could use fseek() except
     * it doesn't work on pipes.  Not clear that being smart is worth
	 * any trouble anyway --- large skips are infrequent.
	*/

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
		  num_bytes -= (long) src->pub.bytes_in_buffer;

		  (void) fill_input_buffer(cinfo);

		  /* note we assume that fill_input_buffer will never return FALSE,
		   * so suspension need not be handled.
		   */
		}

		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF(void)
term_source (j_decompress_ptr cinfo) {
  /* no work necessary here */
}

METHODDEF(void)
term_destination (j_compress_ptr cinfo) {
	freeimage_dst_ptr dest = (freeimage_dst_ptr) cinfo->dest;
	DWORD dwWriteNum = 0;
	size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

	/* Write any data remaining in the buffer */

	if (datacount > 0) {
		//if (dest->m_io->write_proc(dest->buffer, 1, datacount, dest->outfile) != datacount)
		  //throw(cinfo, JERR_FILE_WRITE);
		WriteFile( dest->outfile, dest->buffer, datacount, &dwWriteNum, NULL );
		if( dwWriteNum != datacount )
		{
			longjmp( ( (my_error_ptr)cinfo->err )->setjmp_buffer, JERR_FILE_WRITE );
		}
	}
}

/*
 * Prepare for input from a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing decompression.
 */

static void
jpeg_freeimage_src (j_decompress_ptr cinfo, HANDLE infile )
{
	//, FreeImageIO *io) {
	freeimage_src_ptr src;

	// allocate memory for the buffer. is released automatically in the end

	if (cinfo->src == NULL) {
		cinfo->src = (struct jpeg_source_mgr *) (*cinfo->mem->alloc_small)
			((j_common_ptr) cinfo, JPOOL_PERMANENT, SIZEOF(my_source_mgr));

		src = (freeimage_src_ptr) cinfo->src;

		src->buffer = (JOCTET *) (*cinfo->mem->alloc_small)
			((j_common_ptr) cinfo, JPOOL_PERMANENT, INPUT_BUF_SIZE * SIZEOF(JOCTET));
	}

	// initialize the jpeg pointer struct with pointers to functions

	src = (freeimage_src_ptr) cinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = term_source;
	src->infile = infile;
	//src->m_io = io;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}

static void
jpeg_freeimage_dst (j_compress_ptr cinfo, HANDLE outfile )
{
	//, FreeImageIO *io) {
	freeimage_dst_ptr dest;

	if (cinfo->dest == NULL) {
		cinfo->dest = (struct jpeg_destination_mgr *)(*cinfo->mem->alloc_small)
			((j_common_ptr) cinfo, JPOOL_PERMANENT, SIZEOF(my_destination_mgr));
	}

	dest = (freeimage_dst_ptr) cinfo->dest;
	dest->pub.init_destination = init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
	dest->outfile = outfile;
	//dest->m_io = io;
}

// ==========================================================
// Plugin Interface
// ==========================================================

static int s_format_id;

// ==========================================================
// Plugin Implementation
// ==========================================================
/*
static const char * DLL_CALLCONV
Format() {
	return "JPEG";
}

static const char * DLL_CALLCONV
Description() {
	return "JPEG - JFIF Compliant";
}

static const char * DLL_CALLCONV
Extension() {
	return "jpg,jif,jpeg,jpe";
}

static const char * DLL_CALLCONV
RegExpr() {
	return "^\377\330\377";
}

static const char * DLL_CALLCONV
MimeType() {
	return "image/jpeg";
}

static BOOL DLL_CALLCONV
Validate(FreeImageIO *io, fi_handle handle) {
	BYTE jpeg_signature[] = { 0xFF, 0xD8 };
	BYTE signature[2] = { 0, 0 };

	io->read_proc(signature, 1, sizeof(jpeg_signature), handle);

	return (memcmp(jpeg_signature, signature, sizeof(jpeg_signature)) == 0);
}

static BOOL DLL_CALLCONV
SupportsExportDepth(int depth) {
	return (
			(depth == 8) ||
			(depth == 24)
		);
}

static BOOL DLL_CALLCONV 
SupportsExportType(FREE_IMAGE_TYPE type) {
	return (type == FIT_BITMAP) ? TRUE : FALSE;
}
*/
// ----------------------------------------------------------

static int
CalculateLine(int width, int bitdepth) {
	return ((width * bitdepth) + 7) / 8;
}

static int
CalculatePitch(int line) {
	return line + 3 & ~3;
}

static int
CalculateUsedPaletteEntries(int bit_count) {
	if ((bit_count >= 1) && (bit_count <= 8))
		return 1 << bit_count;

	return 0;
}



//FIBITMAP * DLL_CALLCONV
static HBITMAP
//Load(FreeImageIO *io, fi_handle handle, int page, int flags, void *data) {
Load( HANDLE hFile, int flags, BITMAP * lpBitmap, LPBITMAPINFO lpbmpInfo ) //int page, int flags, void *data) 
{
	HBITMAP hBitmap = NULL;
    BITMAPINFO * lpSrcBitmapInfo = NULL;
	LPBYTE lpBits = NULL;	

	if (hFile) {
		
		//FIBITMAP *dib = NULL;
		//HBITMAP hBitmap;
		DWORD dwSize;

		DWORD dwScanLineBytes;
		long start_pos;
		int bpp;
		struct my_error_mgr jerr;//jpeg_error_mgr jerr;
		struct jpeg_decompress_struct cinfo;

		memset( &cinfo, 0, sizeof(cinfo) );
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit     = jpeg_error_exit;
		jerr.pub.output_message = jpeg_output_message;
		start_pos = SetFilePointer( hFile, 0, NULL, FILE_CURRENT );
		
		if (setjmp(jerr.setjmp_buffer)==0) {
			// remember the start position for EXIF reading later
			
			//io->tell_proc(handle);

			SetFilePointer( hFile, 0, NULL, FILE_BEGIN );
			
			// set up the jpeglib structures
			//
			//			struct jpeg_error_mgr jerr;
			
			// step 1: allocate and initialize JPEG decompression object
			
			//			cinfo.err = jpeg_std_error(&jerr);
			//jerr.setjmp_buffer 
			//			jerr.error_exit     = jpeg_error_exit;
			//			jerr.output_message = jpeg_output_message;
			
			jpeg_create_decompress(&cinfo);
			
			// step 2: specify data source (eg, a handle)
			
			jpeg_freeimage_src(&cinfo, hFile );//, io);
			
			// step 3: read handle parameters with jpeg_read_header()
			
			jpeg_read_header(&cinfo, TRUE);
			
			// step 4a: set parameters for decompression
			
			if ((flags != JPEG_ACCURATE)) {
				cinfo.dct_method          = JDCT_IFAST;
				cinfo.do_fancy_upsampling = FALSE;
			}
			
			// step 4b: allocate dib and init header
			/*
			dib = FreeImage_Allocate(cinfo.image_width, cinfo.image_height, 8 * cinfo.num_components, 0xFF, 0xFF00, 0xFF0000);
			
			  if (cinfo.num_components == 1) {
			  RGBQUAD *colors = FreeImage_GetPalette(dib);
			  
				for (int i = 0; i < 256; i++) {
				colors[i].rgbRed   = i;
				colors[i].rgbGreen = i;
				colors[i].rgbBlue  = i;
				}
				}
				
				  // step 4c: handle metrices
				  
					BITMAPINFOHEADER *pInfoHeader = FreeImage_GetInfoHeader(dib);
					
					  if (cinfo.density_unit == 1) {
					  // dots/inch
					  
						pInfoHeader->biXPelsPerMeter = (int) (((float)cinfo.X_density) / 0.0254000 + 0.5);
						pInfoHeader->biYPelsPerMeter = (int) (((float)cinfo.Y_density) / 0.0254000 + 0.5);
						} else if (cinfo.density_unit == 2) {
						// dots/cm
						
						  pInfoHeader->biXPelsPerMeter = cinfo.X_density * 100;
						  pInfoHeader->biYPelsPerMeter = cinfo.Y_density * 100;
						  }
			*/
			bpp = 8 * cinfo.num_components;
			dwSize = sizeof(BITMAPINFOHEADER);
			dwSize += sizeof(RGBQUAD) * CalculateUsedPaletteEntries(bpp);
			dwScanLineBytes = CalculatePitch(CalculateLine(cinfo.image_width, bpp));
			// lilin remove -2004-12-07, CreateDIBSection 将会分配 bitmap bits
			//dwSize +=  dwScanLineBytes * cinfo.image_height;
			//

			lpSrcBitmapInfo = malloc( dwSize );
			lpBits = malloc( dwScanLineBytes );
			if( lpSrcBitmapInfo && lpBits )
			{
				memset( lpSrcBitmapInfo, 0, dwSize );
				lpSrcBitmapInfo->bmiHeader.biBitCount = bpp;//8 * cinfo.num_components;
				lpSrcBitmapInfo->bmiHeader.biHeight = cinfo.image_height;
				lpSrcBitmapInfo->bmiHeader.biPlanes = 1;
				lpSrcBitmapInfo->bmiHeader.biSize = sizeof(lpSrcBitmapInfo->bmiHeader);
				lpSrcBitmapInfo->bmiHeader.biWidth = cinfo.image_width;
				lpSrcBitmapInfo->bmiHeader.biCompression = BI_RGB;

				//hBitmap = CreateDIBSection( NULL, lpSrcBitmapInfo, DIB_RGB_COLORS, &lpBits, NULL, 0 );
				if( lpbmpInfo )
				{
					if( lpbmpInfo->bmiHeader.biWidth == 0 )
					{
						lpbmpInfo->bmiHeader.biWidth = cinfo.image_width;
					}
					if( lpbmpInfo->bmiHeader.biHeight == 0 )
					{
						lpbmpInfo->bmiHeader.biHeight = cinfo.image_height;
					}
					if( lpbmpInfo->bmiHeader.biBitCount == 0 )
					{
						lpbmpInfo->bmiHeader.biBitCount = bpp;
					}
					hBitmap = CreateDIBSection( NULL, lpbmpInfo, DIB_RGB_COLORS, NULL, NULL, 0 );
				}
				else
					hBitmap = CreateCompatibleBitmap( NULL, cinfo.image_width, cinfo.image_height );

				if( hBitmap )
				{
					LPBYTE lpbScanLine;
					if (cinfo.num_components == 1) 
					{
						int i;
						RGBQUAD *colors = lpSrcBitmapInfo->bmiColors;//  FreeImage_GetPalette(dib);
						
						for (i = 0; i < 256; i++) {
							colors[i].rgbRed   = i;
							colors[i].rgbGreen = i;
							colors[i].rgbBlue  = i;
						}
					}
					
					// step 5: start decompressor
					
					jpeg_start_decompress(&cinfo);
					
					// step 6a: while (scan lines remain to be read) jpeg_read_scanlines(...);
					//倒的图象
					lpbScanLine = lpBits;
					while (cinfo.output_scanline < cinfo.output_height) 
					{
						//lpbScanLine = lpBits + dwScanLineBytes * (cinfo.output_height - cinfo.output_scanline - 1);
						
						//JSAMPROW b = &lpBits;//FreeImage_GetScanLine(dib, cinfo.output_height - cinfo.output_scanline - 1);
						
						jpeg_read_scanlines(&cinfo, &lpbScanLine, 1);
						SetDIBits( NULL, hBitmap, (cinfo.output_height - cinfo.output_scanline), 1 , lpbScanLine, lpSrcBitmapInfo, DIB_RGB_COLORS );
						//lpbScanLine += dwScanLineBytes;
					}
					
					// step 6b: swap red and blue components (see LibJPEG/jmorecfg.h: #define RGB_RED, ...)
					// The default behavior of the JPEG library is kept "as is" because LibTIFF uses 
					// LibJPEG "as is".
					#if RGB_RED == 0

					if(cinfo.num_components == 3) {
						UINT y;
						UINT x;
						lpbScanLine = lpBits;
						for(y = 0; y < cinfo.image_height; y++) {
							BYTE *target = lpbScanLine;//FreeImage_GetScanLine(dib, y);
							BYTE *target_p = target;
							BYTE temp;

							for(x = 0; x < cinfo.image_width; x++) {
								temp = target_p[0];
								target_p[0] = target_p[2];
								target_p[2] = temp;

								//INPLACESWAP(target_p[0], target_p[2]);
								target_p += 3;
							}
							lpbScanLine += dwScanLineBytes;
						}
					}
					#endif
					
					// step 7: finish decompression
					
					jpeg_finish_decompress(&cinfo);
					if( lpBitmap )
					{
						GetObject( hBitmap, sizeof( BITMAP ), lpBitmap );
						/*
						lpBitmap->bmType = 0;
						lpBitmap->bmWidth = (WORD)lpBitmapInfo->bmiHeader.biHeight;
						lpBitmap->bmHeight = (WORD)lpBitmapInfo->bmiHeader.biWidth;
						lpBitmap->bmWidthBytes = 0;
						lpBitmap->bmPlanes = lpBitmapInfo->bmiHeader.biPlanes;
						lpBitmap->bmBitsPixel = lpBitmapInfo->bmiHeader.biBitCount;
						lpBitmap->bmBits = NULL;
						*/
					}
					
					// step 8: release JPEG decompression object
					
					//jpeg_destroy_decompress(&cinfo);
					
					// everything went well. return the loaded dib
					
					///return hBitmap;//(FIBITMAP *)dib;
				}
				//else
				//{
				//	free( lpBitmapInfo );
				//}
			}
			if( lpSrcBitmapInfo )
				free( lpSrcBitmapInfo );
			if( lpBits )
				free( lpBits );
			jpeg_destroy_decompress(&cinfo);
			SetFilePointer( hFile, start_pos, NULL, FILE_BEGIN );
		}
		else
		{
			SetFilePointer( hFile, start_pos, NULL, FILE_BEGIN );
			if( lpSrcBitmapInfo )
				free( lpSrcBitmapInfo );
			if( lpBits )
				free( lpBits );
			if( hBitmap )
				DeleteObject( hBitmap );
			jpeg_destroy_decompress(&cinfo);//FreeImage_Unload(dib);			
		}
	}
	
	return hBitmap;
}

// **************************************************
// 声明：DWORD _Gdi_UnrealizeColor( DWORD dwRealizeColor, DWORD * lpPal, UINT uiPalNum, UINT uiSrcColorType )
// 参数：
// 	IN dwRealizeColor - 设备相关的颜色值
//	IN uiSrcColorType - 颜色类型
//	IN lpPal - 色板指针
//	IN uiPalNum - 色板项目数
// 返回值：
//	假如成功，返回设备无关的颜色值；否则，返回0
// 功能描述：
//	将设备相关的颜色值转化为设备无关的颜色值
// 引用: 
//	
// ************************************************

static DWORD _Gdi_UnrealizeColor( DWORD dwRealizeColor, LPCDWORD lpPal, UINT uiPalNum, UINT uiSrcColorType )
{
	DWORD dwRetv;

	if( uiSrcColorType == PAL_INDEX )
		dwRetv = lpPal[dwRealizeColor];	// 颜色类型为基于色板的索引值
	else if( uiSrcColorType == PAL_BITFIELD )
	{	//颜色类型bit位组合， lpPal里保存有三个 r g b 的 mask值
		int leftShift, rightShift;
		DWORD dwColorMask;

		dwRetv = 0;
		for( rightShift = 24; rightShift >= 0 && uiPalNum; rightShift -= 8, uiPalNum--, lpPal++ )
		{
			dwColorMask = *lpPal;
			//得到 需要移位的值
			for( leftShift = 32; dwColorMask; leftShift-- )
				dwColorMask >>= 1;

			dwRetv |= ( ( dwRealizeColor & *lpPal ) << leftShift ) >> rightShift;
		}
	}
	else
		dwRetv = 0;
	return dwRetv;
}


// **************************************************
// 声明：static DWORD _Gdi_RealizeColor( COLORREF clRgbColor, DWORD * lpPal, UINT uiPalNum, UINT uiDestColorType )
// 参数：
// 	IN clRgbColor - 设备无关的颜色值
//	IN uiDestColorType - 颜色类型
//	IN lpPal - 色板指针
//	IN uiPalNum - 色板项目数
// 返回值：
//	假如成功，返回设备相关的颜色值；否则，返回0
// 功能描述：
//	将设备无关的颜色值转化为设备相关的颜色值
// 引用: 
//	
// ************************************************

// return dest format

static DWORD _Gdi_RealizeColor( COLORREF clRgbColor, LPCDWORD lpPal, UINT uiPalNum, UINT uiDestColorType )
{
	DWORD dwRetv;

	if( uiDestColorType == PAL_INDEX )
	{	// 颜色类型为基于色板的索引值
		long minDiff, curDiff;
		UINT uiPalCount = uiPalNum;
		

		minDiff = 0x7fffffffl;
		dwRetv = uiPalNum;
			
		for( ; uiPalCount; uiPalCount--, lpPal++ )
		{
			if( clRgbColor == *lpPal )
			{
				dwRetv = uiPalCount;
				break;
			}
			else
			{  //得到距离
				LPBYTE lpbSrc = (LPBYTE)&clRgbColor;
				LPBYTE lpbDest = (LPBYTE)lpPal;
				long lDiff;

				curDiff = *lpbSrc++ - *lpbDest++;
				curDiff *= curDiff;
				lDiff = *lpbSrc++ - *lpbDest++;
				curDiff += lDiff * lDiff;
				lDiff = *lpbSrc - *lpbDest;
				curDiff += lDiff * lDiff;				
			}
			if( curDiff < minDiff )
			{	//得到最短距离
			    minDiff = curDiff;
			    dwRetv = uiPalCount;
			}
		}
		dwRetv = uiPalNum - dwRetv;
	}
	else if( uiDestColorType == PAL_BITFIELD )
	{	//颜色类型bit位组合， lpPal里保存有三个 r g b 的 mask值
		int leftShift, rightShift;
		DWORD dwColorMask;

		dwRetv = 0;
		for( leftShift = 24; leftShift >= 0 && uiPalNum; leftShift -= 8, uiPalNum--, lpPal++ )
		{
			dwColorMask = *lpPal;
			//得到 需要移位的值
			for( rightShift = 32; dwColorMask; rightShift-- )
				dwColorMask >>= 1;

			dwRetv |= ( ( ( clRgbColor & *lpPal ) << leftShift ) >> rightShift ) & *lpPal;
		}
	}
	else
		dwRetv = 0;
	return dwRetv;
}

static void 
ConvertLine8To24(BYTE *target, BYTE *source, int width_in_pixels, RGBQUAD *palette) 
{
	int cols;
	for (cols = 0; cols < width_in_pixels; cols++) {
		target[0] = palette[source[cols]].rgbBlue;
		target[1] = palette[source[cols]].rgbGreen;
		target[2] = palette[source[cols]].rgbRed;

		target += 3;
	}
}

// 2004-10-19, add by lilin
static void 
ConvertLineBitFieldTo24(BYTE *target, BYTE *source, int width_in_pixels, RGBQUAD *palette) 
{
	int cols;
	LPWORD lpwSource = (LPWORD)source;
	DWORD  dwRgb;
//	COLORREF d;
	for (cols = 0; cols < width_in_pixels; cols++) {
		dwRgb = _Gdi_UnrealizeColor( lpwSource[cols], (LPCDWORD)palette, 3, PAL_BITFIELD );
		target[0] = GetBValue(dwRgb);//palette[source[cols]].rgbBlue;
		target[1] = GetGValue(dwRgb);//palette[source[cols]].rgbGreen;
		target[2] = GetRValue(dwRgb);//palette[source[cols]].rgbRed;

		target += 3;
	}
}


static RGBQUAD * GetPalette( LPBITMAPINFO dib ) 
{
	return (dib && dib->bmiHeader.biBitCount < 16 ) ? (RGBQUAD *)dib->bmiColors : NULL;
}

enum FREE_IMAGE_COLOR_TYPE 
{
	FIC_MINISWHITE = 0,		// min value is white
    FIC_MINISBLACK = 1,		// min value is black
    FIC_RGB        = 2,		// RGB color model
    FIC_PALETTE    = 3,		// color map indexed
	FIC_RGBALPHA   = 4,		// RGB color model with alpha channel
	FIC_CMYK       = 5		// CMYK color model
};

static enum FREE_IMAGE_COLOR_TYPE GetColorType( LPBITMAPINFO dib ) 
{
	RGBQUAD *rgb;

	// special bitmap type
//	if(GetImageType(dib) != FIT_BITMAP) {
//		return FIC_MINISBLACK;
//	}

	// standard image type
	switch (dib->bmiHeader.biBitCount) {
		case 1:
		{
			rgb = GetPalette(dib);

			if ((rgb->rgbRed == 0) && (rgb->rgbGreen == 0) && (rgb->rgbBlue == 0)) {
				rgb++;

				if ((rgb->rgbRed == 255) && (rgb->rgbGreen == 255) && (rgb->rgbBlue == 255))
					return FIC_MINISBLACK;				
			}

			if ((rgb->rgbRed == 255) && (rgb->rgbGreen == 255) && (rgb->rgbBlue == 255)) {
				rgb++;

				if ((rgb->rgbRed == 0) && (rgb->rgbGreen == 0) && (rgb->rgbBlue == 0))
					return FIC_MINISWHITE;				
			}

			return FIC_PALETTE;
		}

		case 4:
		case 8:	// Check if the DIB has a color or a greyscale palette
		{
			int ncolors = dib->bmiHeader.biClrUsed;//FreeImage_GetColorsUsed(dib);
		    int minisblack = 1;
			int i;
			rgb = GetPalette(dib);

			for (i = 0; i < ncolors; i++) {
				if ((rgb->rgbRed != rgb->rgbGreen) || (rgb->rgbRed != rgb->rgbBlue))
					return FIC_PALETTE;

				// The DIB has a color palette if the greyscale isn't a linear ramp
				// Take care of reversed grey images
				if (rgb->rgbRed != i) {
					if ((ncolors-i-1) != rgb->rgbRed)
						return FIC_PALETTE;
				    else
						minisblack = 0;
				}

				rgb++;
			}

			return minisblack ? FIC_MINISBLACK : FIC_MINISWHITE;
		}

		case 16:
		case 24:
			return FIC_RGB;

		case 32:
		{
		//	if (FreeImage_GetICCProfile(dib)->flags & FIICC_COLOR_IS_CMYK)
		//		return FIC_CMYK;

			//for (unsigned y = 0; y < FreeImage_GetHeight(dib); y++) {
			//	rgb = (RGBQUAD *)FreeImage_GetScanLine(dib, y);

			//	for (unsigned x = 0; x < FreeImage_GetWidth(dib); x++)
			//		if (rgb[x].rgbReserved != 0xFF)
			//			return FIC_RGBALPHA;			
			//}

			return FIC_RGB;
		}
				
		default :
			return FIC_MINISBLACK;
	}
}


static BOOL Save( HANDLE hFile, BITMAPINFO * lpbmpInfo,  LPBYTE lpBits, UINT flags ) 
{
	if( lpbmpInfo && hFile ) 
	{
		struct my_error_mgr jerr;//jpeg_error_mgr jerr;
		struct jpeg_compress_struct cinfo;
		BYTE *target = NULL;

		
		memset( &cinfo, 0, sizeof(cinfo) );
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit     = jpeg_error_exit;
		jerr.pub.output_message = jpeg_output_message;
		
		//try {
			// Check dib format
		if (setjmp(jerr.setjmp_buffer)==0) 
		{
			BITMAPINFOHEADER *pInfoHeader;
			int quality;
			const char *sError = "only 24-bit highcolor or 8-bit greyscale/palette bitmaps can be saved as JPEG";

			enum FREE_IMAGE_COLOR_TYPE color_type = GetColorType(lpbmpInfo);
			WORD bpp = lpbmpInfo->bmiHeader.biBitCount;//FreeImage_GetBPP(dib);

			if ((bpp != 24) && (bpp != 8) && (bpp != 16) )
				return FALSE;//throw sError;

			if(bpp == 8) {
				// allow grey, reverse grey and palette 
				if ((color_type != FIC_MINISBLACK) && (color_type != FIC_MINISWHITE) && (color_type != FIC_PALETTE))
					return FALSE;//throw sError;
			}


		//	struct jpeg_compress_struct cinfo;
		//	struct jpeg_error_mgr jerr;

			// Step 1: allocate and initialize JPEG compression object

//			cinfo.err = jpeg_std_error(&jerr);

//			jerr.error_exit     = jpeg_error_exit;
//			jerr.output_message = jpeg_output_message;

			// Now we can initialize the JPEG compression object.

			jpeg_create_compress(&cinfo);

			// Step 2: specify data destination (eg, a file) 

			jpeg_freeimage_dst(&cinfo, hFile);//andle);//, io);

			// Step 3: set parameters for compression

			cinfo.image_width = lpbmpInfo->bmiHeader.biWidth;//FreeImage_GetWidth(dib);
			cinfo.image_height = lpbmpInfo->bmiHeader.biHeight;//FreeImage_GetHeight(dib);

			switch(color_type) {
				case FIC_MINISBLACK :
				case FIC_MINISWHITE :
					cinfo.in_color_space = JCS_GRAYSCALE;
					cinfo.input_components = 1;
					break;

				default :
					cinfo.in_color_space = JCS_RGB;
					cinfo.input_components = 3;
					break;
			}

			jpeg_set_defaults(&cinfo);

			// Set JFIF density parameters from the DIB data

			pInfoHeader = &lpbmpInfo->bmiHeader;//FreeImage_GetInfoHeader(dib);
			cinfo.X_density = (WORD)(pInfoHeader->biXPelsPerMeter / 100.0F + 0.5);
			cinfo.Y_density = (WORD)(pInfoHeader->biYPelsPerMeter / 100.0F + 0.5);
			cinfo.density_unit = 2;	// dots / cm

			// Step 4: set quality
			// the first 7 bits are reserved for low level quality settings
			// the other bits are high level (i.e. enum-ish)

			

			if ((flags & JPEG_QUALITYBAD) == JPEG_QUALITYBAD) {
				quality = 10;
			} else if ((flags & JPEG_QUALITYAVERAGE) == JPEG_QUALITYAVERAGE) {
				quality = 25;
			} else if ((flags & JPEG_QUALITYNORMAL) == JPEG_QUALITYNORMAL) {
				quality = 50;
			} else if ((flags & JPEG_QUALITYGOOD) == JPEG_QUALITYGOOD) {
				quality = 75;
			} else 	if ((flags & JPEG_QUALITYSUPERB) == JPEG_QUALITYSUPERB) {
				quality = 100;
			} else {
				if ((flags & 0x7F) == 0) {
					quality = 75;
				} else {
					quality = flags & 0x7F;
				}
			}

			jpeg_set_quality(&cinfo, quality, TRUE); // limit to baseline-JPEG values

			// Step 5: Start compressor

			jpeg_start_compress(&cinfo, TRUE);

			// Step 6: while (scan lines remain to be written)

			if(color_type == FIC_RGB) {
				// 24-bit or 16-bit or 32-bit RGB image : need to swap red and blue channels
				unsigned pitch = CalculatePitch(CalculateLine(cinfo.image_width, bpp));//FreeImage_GetPitch(dib);
				RGBQUAD *palette = lpbmpInfo->bmiColors;
				LPBYTE lpbScanLine;
				LPBYTE lpbTarget = NULL;
				if( bpp != 24 )
				{
					lpbTarget = (BYTE*)malloc(cinfo.image_width * 3);
				}

//				BYTE *target = (BYTE*)malloc(pitch * sizeof(BYTE));
				if( bpp == 24 || lpbTarget )
				{
					while (cinfo.next_scanline < cinfo.image_height) {
						// get a copy of the scanline
						//memcpy(target, FreeImage_GetScanLine(dib, FreeImage_GetHeight(dib) - cinfo.next_scanline - 1), pitch);
						lpbScanLine = lpBits + (cinfo.image_height - cinfo.next_scanline - 1) * pitch;
						//					memcpy(target, lpbScanLine, pitch);
						
#if RGB_RED == 0
						// swap R and B channels
						
						BYTE *target_p = target;
						for(int x = 0; x < cinfo.image_width; x++) {
							INPLACESWAP(target_p[0], target_p[2]);
							target_p += 3;
						}
#endif
						// write the scanline
						//					jpeg_write_scanlines(&cinfo, &target, 1);
						if( bpp != 24 )
						{
						    ConvertLineBitFieldTo24(lpbTarget, lpbScanLine, cinfo.image_width, palette);
							lpbScanLine = lpbTarget;
						}
						jpeg_write_scanlines(&cinfo, &lpbScanLine, 1);
					}
				}
				if( lpbTarget )
					free(lpbTarget);
				//free(target);
			}
			else if(color_type == FIC_MINISBLACK) {
				unsigned pitch = CalculatePitch(CalculateLine(cinfo.image_width, bpp));//FreeImage_GetPitch(dib);
				LPBYTE lpbScanLine;

				// 8-bit standard greyscale images
				while (cinfo.next_scanline < cinfo.image_height) {
//					JSAMPROW b = FreeImage_GetScanLine(dib, FreeImage_GetHeight(dib) - cinfo.next_scanline - 1);
//					jpeg_write_scanlines(&cinfo, &b, 1);
					lpbScanLine = lpBits + (cinfo.image_height - cinfo.next_scanline - 1) * pitch;
					jpeg_write_scanlines(&cinfo, &lpbScanLine, 1);
				}
			}
			else if(color_type == FIC_PALETTE) {
				// 8-bit palettized images are converted to 24-bit images
				RGBQUAD *palette = lpbmpInfo->bmiColors;// FreeImage_GetPalette(dib);
				target = (BYTE*)malloc(cinfo.image_width * 3);
				if (target) // == NULL)
				{
					unsigned pitch = CalculatePitch(CalculateLine(cinfo.image_width, bpp));//FreeImage_GetPitch(dib);
					LPBYTE lpbScanLine;

					//throw "no memory to allocate intermediate scanline buffer";
					while (cinfo.next_scanline < cinfo.image_height) {
//						BYTE *source = FreeImage_GetScanLine(dib, FreeImage_GetHeight(dib) - cinfo.next_scanline - 1);
						lpbScanLine = lpBits + (cinfo.image_height - cinfo.next_scanline - 1) * pitch;

						ConvertLine8To24(target, lpbScanLine, cinfo.image_width, palette);
						
						jpeg_write_scanlines(&cinfo, &target, 1);
					}
					lpbScanLine = target;
					target = NULL; // for setjmp safe
					free(lpbScanLine);					
				}				
			}
			else if(color_type == FIC_MINISWHITE) {
				// reverse 8-bit greyscale image, so reverse grey value on the fly
				UINT i;
				BYTE reverse[256];
				target = (BYTE *)malloc(cinfo.image_width);
				if (target)// == NULL)
				{
					//	throw "no memory to allocate intermediate scanline buffer";
					unsigned pitch = CalculatePitch(CalculateLine(cinfo.image_width, bpp));//FreeImage_GetPitch(dib);
					LPBYTE lpbScanLine;
					
					for(i = 0; i < 256; i++) {
						reverse[i] = 255 - i;
					}
					
					while(cinfo.next_scanline < cinfo.image_height) {
//						BYTE *source = FreeImage_GetScanLine(dib, FreeImage_GetHeight(dib) - cinfo.next_scanline - 1);
						lpbScanLine = lpBits + (cinfo.image_height - cinfo.next_scanline - 1) * pitch;

						for(i = 0; i < cinfo.image_width; i++) {
							target[i] = reverse[ lpbScanLine[i] ];
						}
						jpeg_write_scanlines(&cinfo, &target, 1);
					}
					lpbScanLine = target;
					target = NULL;	//for setjmp safe
					free(lpbScanLine);
				}
			}

			// Step 7: Finish compression 

			jpeg_finish_compress(&cinfo);

			// Step 8: release JPEG compression object 

			jpeg_destroy_compress(&cinfo);

			return TRUE;

		}
		else
		{
			if( target )
			{
				free( target );
				target = NULL;
			}
			jpeg_destroy_compress(&cinfo);
			//FreeImage_OutputMessageProc(s_format_id, text);
#ifdef KINGMOS
			RETAILMSG( 1, ( "error in save jpeg.\r\n" ) );
#endif
			return FALSE;
		}
	}

	return FALSE;
}


// ==========================================================
//   Init
// ==========================================================
/*
void DLL_CALLCONV
InitJPEG(Plugin *plugin, int format_id) {
	s_format_id = format_id;

	plugin->format_proc = Format;
	plugin->description_proc = Description;
	plugin->extension_proc = Extension;
	plugin->regexpr_proc = RegExpr;
	plugin->open_proc = NULL;
	plugin->close_proc = NULL;
	plugin->pagecount_proc = NULL;
	plugin->pagecapability_proc = NULL;
	plugin->load_proc = Load;
	plugin->save_proc = Save;
	plugin->validate_proc = Validate;
	plugin->mime_proc = MimeType;
	plugin->supports_export_bpp_proc = SupportsExportDepth;
	plugin->supports_export_type_proc = SupportsExportType;
	plugin->supports_icc_profiles_proc = NULL;	// not implemented yet;
}
*/

BOOL WINAPI JPEG_Init( void )
{
	return TRUE;
}

BOOL WINAPI JPEG_Deinit( void )
{
	return TRUE;
}

HBITMAP WINAPI JPEG_LoadByHandleEx( HANDLE hFile, BITMAP * lpBitmap, LPBITMAPINFO lpbmpInfo )
{
	if( hFile && hFile != INVALID_HANDLE_VALUE )
		return Load( hFile, 0, lpBitmap, lpbmpInfo );
	return NULL;
}

HBITMAP WINAPI JPEG_LoadByHandle( HANDLE hFile, BITMAP * lpBitmap )
{
	return JPEG_LoadByHandleEx( hFile, lpBitmap, NULL );
}

HBITMAP WINAPI JPEG_LoadByNameEx( LPCTSTR lpszFileName, BITMAP * lpBitmap, LPBITMAPINFO lpbmpInfo )
{
	HBITMAP hbmpRetv = NULL;
	if( lpszFileName )
	{
		HANDLE hFile = CreateFile( lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
		if( hFile != INVALID_HANDLE_VALUE )
		{
		    hbmpRetv = Load( hFile, 0, lpBitmap, lpbmpInfo );
			CloseHandle( hFile );
		}
	}
	return hbmpRetv;
}

HBITMAP WINAPI JPEG_LoadByName( LPCTSTR lpszFileName, BITMAP * lpBitmap )
{
	return JPEG_LoadByNameEx( lpszFileName, lpBitmap, NULL );
}

BOOL WINAPI JPEG_SaveByHandle( HANDLE hFile, LPBITMAPINFO lpbmpInfo, LPVOID lpBits, UINT flags )
{
	BOOL bRetv = FALSE;
	if( hFile && hFile != INVALID_HANDLE_VALUE && lpbmpInfo && lpBits )
	{
		SetFilePointer( hFile, 0, NULL, FILE_BEGIN );
	    bRetv = Save( hFile, lpbmpInfo, lpBits, flags );
		SetFilePointer( hFile, 0, NULL, FILE_BEGIN );
	}
	return bRetv; 
}

BOOL WINAPI JPEG_SaveByName( LPCTSTR lpszFileName, LPBITMAPINFO lpbmpInfo, LPVOID lpBits, UINT flags )
{
	BOOL bRetv = FALSE;
	if( lpszFileName && lpbmpInfo && lpBits )
	{
		HANDLE hFile = CreateFile( lpszFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
		if( hFile != INVALID_HANDLE_VALUE )
		{
		    bRetv = Save( hFile, lpbmpInfo, lpBits, flags );
			CloseHandle( hFile );
		}
	}
	return bRetv; 
}

