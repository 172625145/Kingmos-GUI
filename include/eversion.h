/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EVERSION_H
#define __EVERSION_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

// version format major.minor
#define MAJOR_VERSION   3
#define MINOR_VERSION   0
#define BUILD_VERSION   0
// format :   msb
#define MAKE_VERSION( major, minor, build )  ( (((build) & 0xf) << 16) | \
	                                           ( (major) << 8) | \
											   (minor) )
#define GET_MINOR_VERSION( verion ) ( (version) & 0xff )
#define GET_MAJOR_VERSION( verion ) ( ( (version) >> 8 ) & 0xff )
#define GET_BUILD_VERSION( verion ) ( ( (version) >> 16 ) & 0xf )

#define KINGMOS_VERSION MAKE_VERSION( MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION )

#ifndef KINGMOS
#define KINGMOS
#endif

#define __DEBUG
//#define SINGLE_THREAD

/*BEGIN defines compiler type for build this os */
/* GNU compiler */
//#define __GNUC__
//#define __WINDOWS_
/* MS compiler for windows */
//#define __MSWC__
/* MS compiler for dos */
//#define __MSDC__
/* Borland compiler */
//#define __BLDC__
/*END defines compiler type for build this os */


/*BEGIN defines CPU type for this os build for */
/* arm cpu */

/* x86 cpu */
//#define X86_CPU
/* 68k cpu */
//#define 68K_CPU
/* MIPS cpu */
//#define MIPS_CPU
/*END defines CPU type for this os build for */
#ifdef  __CC_ARM
	#define ARM_CPU
#endif

#ifdef VC386
    #define __MSWC__
    #define X86_CPU
#endif

#ifdef WINCE_ARM
    #define __MSWC__
	#define ARM_CPU
#endif
//#ifdef ARM
  //  #define ARM_CPU
//#endif

#ifdef __GNUC__
    #define ARM_CPU
    #define ARM
#endif


/* defines for build os, take care this */

#ifdef __GNUC__

	#ifdef ARM_CPU
		#define __GNUC__ARM__
	#elif defined X86_CPU
		#define __GNUC__X86__
	#elif defined MIPS_CPU
		#define __GNUC__MIPS__
	#else
		#error "You must select CPU type"
    #endif  /*endif __GNUC__*/

#elif defined __MSWC__

	#ifdef ARM_CPU
		#define __MSWC__ARM__
	#elif defined X86_CPU
		#define __MSWC__X86__
	#else
		#error "You must select CPU type"
    #endif  /*endif __MSWC__*/

#elif defined __CC_ARM
    #define __ARMCC__ARM__
#else

	#error "You must select compiler type"

#endif

#ifdef __MSDC__
	//#define CPU_X86
	#define EML_DOS
	//#define COLOR_1BPP
	//#define COLOR_2BPP
	//#define COLOR_8BPP
	#define COLOR_2PLANE
	#ifdef COLOR_4PLANE
		#define COLOR_PLANE
	#endif

	#ifdef COLOR_2PLANE
		#define COLOR_PLANE
	#endif
#endif  //__MSDC__

#ifdef __MSWC__X86__
	//#define CPU_X86
	#define EML_WIN32
	//#define COLOR_1BPP
    //#define COLOR_4BPP
	//#define COLOR_8BPP
	#define COLOR_16BPP
    
	//#define V160X128
	//#define V128X160
	//#define V160X160
	//#define V320X200
	//#define V640X480
	//#define V800X600
	//#define V240X320
	#define V840X480
	
	#define TIMER_RESCHE
    #define INLINE_PROGRAM

#endif   //__MSWC__X86__

#ifdef __MSWC__ARM__
	//#define CPU_ARM
	#define COLOR_16BPP
	#define NOT_EML
	#define V800X600
	#define VIRTUAL_MEM
	#define TIMER_RESCHE
    #define INLINE_PROGRAM
    #define CALL_TRAP
#endif  //__MSWC__ARM__

#ifdef __GNUC__ARM__

#ifdef SA11X0

	#define COLOR_16BPP
	#define NOT_EML
	#define V800X600
	#define VIRTUAL_MEM
	#define TIMER_RESCHE
//    #define INLINE_PROGRAM
    #define CALL_TRAP

#elif defined ARM7TDMI
//#ifdef ARM7TDMI
	#define COLOR_4BPP
	#define NOT_EML
	#define V320X240
	//#define VIRTUAL_MEM
	#define TIMER_RESCHE
    #define INLINE_PROGRAM
    #define CALL_TRAP
//#endif

#elif defined S3C2410

	#define COLOR_16BPP
	#define NOT_EML
	#define V240X320
	#define VIRTUAL_MEM
	#define TIMER_RESCHE
    #define CALL_TRAP

#elif defined AU1200

	#define COLOR_16BPP
	#define NOT_EML
	#define V840X480
//	#define VIRTUAL_MEM
	#define TIMER_RESCHE
//    #define CALL_TRAP
    #define INLINE_PROGRAM
#else
	#error "You must select target cpu!"
#endif   

#endif  //__GNUC__ARM__

#ifdef __ARMCC__ARM__
	#define COLOR_4BPP
	#define NOT_EML
	#define V320X200
//	#define VIRTUAL_MEM
	#define TIMER_RESCHE
    #define INLINE_PROGRAM
    #define CALL_TRAP
#endif  //__GNUC__ARM__

#ifdef M68K
	#define CPU_M68K
	#define COLOR_2BPP
	#define NOT_EML
#endif

//#define CPU_68K
//#define EML_ADS


#define CHINESE_SIMPLIFIED
//#define ENGLISH_AMERICA


#ifdef COLOR_1BPP
    #define BPP 1
#endif

#ifdef COLOR_2BPP
    #define BPP 2
#endif

#ifdef COLOR_4BPP
    #define BPP 4
#endif

#ifdef COLOR_8BPP
    #define BPP 8
#endif

#ifdef COLOR_16BPP
    #define BPP 16
#endif

#ifdef COLOR_32BPP
    #define BPP 32
#endif

#ifdef V160X128
#define DISPLAY_WIDTH 160
#define DISPLAY_HEIGHT 128
#endif

#ifdef V160X160
#define DISPLAY_WIDTH 160
#define DISPLAY_HEIGHT 160
#endif

#ifdef V320X200
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 200
#endif

#ifdef V640X480
#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480
#define LARGE_SCREEN
#endif

#ifdef V800X600
#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600
#define LARGE_SCREEN
#endif

#ifdef V240X320
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320
#define LARGE_SCREEN
#endif

#ifdef V840X480
//#define DISPLAY_WIDTH 840
//#define DISPLAY_HEIGHT 480
//#define LARGE_SCREEN
#endif

#ifdef ZT_PHONE
	#define AP_XSTART     0
	#define AP_YSTART     0
	#define AP_WIDTH      240
	#define AP_HEIGHT     320
	#define USE_24X24_CHINESE_FONT
#else
	#define AP_XSTART     30
	#define AP_YSTART     0
	#define AP_WIDTH      210
	#define AP_HEIGHT     300
#endif
//#define FAT_32
//#define FAT_64
//#define FAT_128


#undef KINGMOS_UNICODE

#ifdef KINGMOS_CORE
	#define HAVE_GDC_1BPP
	#define HAVE_GDC_16BPP
	#define HAVE_GDC_32BPP

	#define HAVE_8X8_ENG_FIXED
	#define HAVE_8X6_ENG_FIXED
	#define HAVE_16X16_CHS_FIXED    //include 8x16 english
	#define HAVE_16X16_SYMBOL_FIXED
	#define HAVE_8X16_PHONETIC_FIXED

#elif defined KINGMOS_STUDY
	#define HAVE_ALL_FONT
	#define HAVE_ALL_GDC
#elif defined KINGMOS_EVI
	#define HAVE_ALL_FONT
	#define HAVE_ALL_GDC
#elif defined KINGMOS_EVII
	#define HAVE_ALL_FONT
	#define HAVE_ALL_GDC
#elif defined KINGMOS_FULL
	#define HAVE_ALL_FONT
	#define HAVE_ALL_GDC
#else
	#define HAVE_ALL_FONT
	#define HAVE_ALL_GDC
#endif

#ifdef HAVE_ALL_GDC
	#define HAVE_GDC_1BPP
	#define HAVE_GDC_4BPP
	#define HAVE_GDC_8BPP
	#define HAVE_GDC_16BPP
	#define HAVE_GDC_24BPP
	#define HAVE_GDC_32BPP
#endif

#ifdef HAVE_ALL_FONT

	#define HAVE_8X8_ENG_FIXED
	#define HAVE_8X6_ENG_FIXED
	#define HAVE_16X16_CHS_FIXED    //include 8x16 english
	#define HAVE_24X24_CHS_FIXED	//include 12x24 english
	#define HAVE_16X16_SYMBOL_FIXED
	#define HAVE_24X24_SYMBOL_FIXED
	#define HAVE_8X16_PHONETIC_FIXED
	#define HAVE_TRUETYPE

#endif

#ifdef KINGMOS_KERNEL

#elif defined WINDOWS_KERNEL

#elif defined LINUX_KERNEL

#else
	#error "You must select kernel type"
#endif



#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif   // __EVERSION_H
