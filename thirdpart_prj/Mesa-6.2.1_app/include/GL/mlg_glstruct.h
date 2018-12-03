#ifndef _MLG_GLSTRUCT
#define _MLG_GLSTRUCT


#ifdef  __cplusplus
extern "C" {
#endif

// 定义微软有定义但是MLG没有定义的结构
#include "erect.h"
//#include "egdi.h"
// 根据微软的结构来定义属于OPENGL的专用结构
struct tagPIXELFORMATDESCRIPTOR{
  WORD  nSize; 
  WORD  nVersion; 
  DWORD dwFlags; 
  BYTE  iPixelType; 
  BYTE  cColorBits; 
  BYTE  cRedBits; 
  BYTE  cRedShift; 
  BYTE  cGreenBits; 
  BYTE  cGreenShift; 
  BYTE  cBlueBits; 
  BYTE  cBlueShift; 
  BYTE  cAlphaBits; 
  BYTE  cAlphaShift; 
  BYTE  cAccumBits; 
  BYTE  cAccumRedBits; 
  BYTE  cAccumGreenBits; 
  BYTE  cAccumBlueBits; 
  BYTE  cAccumAlphaBits; 
  BYTE  cDepthBits; 
  BYTE  cStencilBits; 
  BYTE  cAuxBuffers; 
  BYTE  iLayerType; 
  BYTE  bReserved; 
  DWORD dwLayerMask; 
  DWORD dwVisibleMask; 
  DWORD dwDamageMask; 
};//PIXELFORMATDESCRIPTOR;
typedef	struct		tagPIXELFORMATDESCRIPTOR	PIXELFORMATDESCRIPTOR, *LPPIXELFORMATDESCRIPTOR;

/* pixel types */
#define PFD_TYPE_RGBA        0
#define PFD_TYPE_COLORINDEX  1

/* layer types */
#define PFD_MAIN_PLANE       0
#define PFD_OVERLAY_PLANE    1
#define PFD_UNDERLAY_PLANE   (-1)

/* PIXELFORMATDESCRIPTOR flags */
#define PFD_DOUBLEBUFFER            0x00000001
#define PFD_STEREO                  0x00000002
#define PFD_DRAW_TO_WINDOW          0x00000004
#define PFD_DRAW_TO_BITMAP          0x00000008
#define PFD_SUPPORT_GDI             0x00000010
#define PFD_SUPPORT_OPENGL          0x00000020
#define PFD_GENERIC_FORMAT          0x00000040
#define PFD_NEED_PALETTE            0x00000080
#define PFD_NEED_SYSTEM_PALETTE     0x00000100
#define PFD_SWAP_EXCHANGE           0x00000200
#define PFD_SWAP_COPY               0x00000400
#define PFD_SWAP_LAYER_BUFFERS      0x00000800
#define PFD_GENERIC_ACCELERATED     0x00001000
#define PFD_SUPPORT_DIRECTDRAW      0x00002000

/* PIXELFORMATDESCRIPTOR flags for use in ChoosePixelFormat only */
#define PFD_DEPTH_DONTCARE          0x20000000
#define PFD_DOUBLEBUFFER_DONTCARE   0x40000000
#define PFD_STEREO_DONTCARE         0x80000000

// For gdi->wgl.c 定义，实现在微软中的定义
typedef struct _FIXED {
#ifndef _MAC
    WORD    fract;
    short   value;
#else
    short   value;
    WORD    fract;
#endif
} FIXED;

// For dgi->wgl.c中定义
typedef struct _GLYPHMETRICS {
    UINT    gmBlackBoxX;
    UINT    gmBlackBoxY;
    POINT   gmptGlyphOrigin;
    short   gmCellIncX;
    short   gmCellIncY;
} GLYPHMETRICS, FAR *LPGLYPHMETRICS;

// For dgi->wgl.c中定义
typedef struct _MAT2 {
     FIXED  eM11;
     FIXED  eM12;
     FIXED  eM21;
     FIXED  eM22;
} MAT2, FAR *LPMAT2;

//  GetGlyphOutline constants

#define GGO_METRICS        0
#define GGO_BITMAP         1
#define GGO_NATIVE         2
#define GGO_BEZIER         3

/* Device Parameters for GetDeviceCaps() */
#define DRIVERVERSION 0     /* Device driver version                    */
#define TECHNOLOGY    2     /* Device classification                    */
#define HORZSIZE      4     /* Horizontal size in millimeters           */
#define VERTSIZE      6     /* Vertical size in millimeters             */
#define HORZRES       8     /* Horizontal width in pixels               */
#define VERTRES       10    /* Vertical height in pixels                */
#define BITSPIXEL     12    /* Number of bits per pixel                 */
#define PLANES        14    /* Number of planes                         */
#define NUMBRUSHES    16    /* Number of brushes the device has         */
#define NUMPENS       18    /* Number of pens the device has            */
#define NUMMARKERS    20    /* Number of markers the device has         */
#define NUMFONTS      22    /* Number of fonts the device has           */
#define NUMCOLORS     24    /* Number of colors the device supports     */
#define PDEVICESIZE   26    /* Size required for device descriptor      */
#define CURVECAPS     28    /* Curve capabilities                       */
#define LINECAPS      30    /* Line capabilities                        */
#define POLYGONALCAPS 32    /* Polygonal capabilities                   */
#define TEXTCAPS      34    /* Text capabilities                        */
#define CLIPCAPS      36    /* Clipping capabilities                    */
#define RASTERCAPS    38    /* Bitblt capabilities                      */
#define ASPECTX       40    /* Length of the X leg                      */
#define ASPECTY       42    /* Length of the Y leg                      */
#define ASPECTXY      44    /* Length of the hypotenuse                 */

/* Logical Palette */
/*typedef struct tagLOGPALETTE {
    WORD        palVersion;
    WORD        palNumEntries;
    PALETTEENTRY        palPalEntry[1];
} LOGPALETTE, *PLOGPALETTE, NEAR *NPLOGPALETTE, FAR *LPLOGPALETTE;*/

typedef struct _POINTL      /* ptl  */
{
    LONG  x;
    LONG  y;
} POINTL, *PPOINTL;
#define CCHDEVICENAME 32
#define CCHFORMNAME 32
typedef struct _devicemodea {
    BYTE   dmDeviceName[CCHDEVICENAME];
    WORD dmSpecVersion;
    WORD dmDriverVersion;
    WORD dmSize;
    WORD dmDriverExtra;
    DWORD dmFields;
    union {
      struct {
        short dmOrientation;
        short dmPaperSize;
        short dmPaperLength;
        short dmPaperWidth;
      };
      POINTL dmPosition;
    };
    short dmScale;
    short dmCopies;
    short dmDefaultSource;
    short dmPrintQuality;
    short dmColor;
    short dmDuplex;
    short dmYResolution;
    short dmTTOption;
    short dmCollate;
    BYTE   dmFormName[CCHFORMNAME];
    WORD   dmLogPixels;
    DWORD  dmBitsPerPel;
    DWORD  dmPelsWidth;
    DWORD  dmPelsHeight;
    DWORD  dmDisplayFlags;
    DWORD  dmDisplayFrequency;
#if(WINVER >= 0x0400)
    DWORD  dmICMMethod;
    DWORD  dmICMIntent;
    DWORD  dmMediaType;
    DWORD  dmDitherType;
    DWORD  dmReserved1;
    DWORD  dmReserved2;
#if (WINVER >= 0x0500) || (_WIN32_WINNT >= 0x0400)
    DWORD  dmPanningWidth;
    DWORD  dmPanningHeight;
#endif
#endif /* WINVER >= 0x0400 */
} DEVMODEA, *PDEVMODEA, *NPDEVMODEA, *LPDEVMODEA;

// To use in the file of mipmap.c of GLU Project
#define UINT_MAX      0xffffffff    /* maximum unsigned int value */
#define INT_MAX       2147483647    /* maximum (signed) int value */
#define LONG_MAX      2147483647L   /* maximum (signed) long value */

// 注释空函数
#define		printf		((void)0)
#define		vsnprintf	((void)0)

#define		abort()		((void)0)
#define		exit(EX)		((void)0)

// 声明句柄
//DEFINE_HANDLE( HPALETTE );	// Added By Liujun


#ifdef  __cplusplus
}
#endif

#endif
