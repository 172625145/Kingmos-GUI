# Microsoft Developer Studio Project File - Name="freetype" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=freetype - Win32 Debug Singlethreaded
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "freetype.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "freetype.mak" CFG="freetype - Win32 Debug Singlethreaded"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "freetype - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "freetype - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "freetype - Win32 Debug Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "freetype - Win32 Release Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "freetype - Win32 Release Singlethreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "freetype - Win32 Debug Singlethreaded" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "freetype - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\objs\release"
# PROP Intermediate_Dir "..\..\..\objs\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /MD /Za /W4 /GX /Zi /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /nologo /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\objs\freetype2110.lib"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\objs\debug"
# PROP Intermediate_Dir "..\..\..\objs\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /MDd /Za /W4 /GX /Zi /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "FT_DEBUG_LEVEL_ERROR" /D "FT_DEBUG_LEVEL_TRACE" /FD /GZ /c
# SUBTRACT CPP /nologo /X /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\objs\freetype2110_D.lib"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "freetype___Win32_Debug_Multithreaded"
# PROP BASE Intermediate_Dir "freetype___Win32_Debug_Multithreaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\objs\debug_mt"
# PROP Intermediate_Dir "..\..\..\objs\debug_mt"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Za /W3 /Gm /GX /ZI /Od /I "..\freetype\include\\" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "FT_FLAT_COMPILE" /YX /FD /GZ /c
# SUBTRACT BASE CPP /X
# ADD CPP /MTd /Za /W4 /GX /Zi /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "FT_DEBUG_LEVEL_ERROR" /D "FT_DEBUG_LEVEL_TRACE" /FD /GZ /c
# SUBTRACT CPP /nologo /X /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\freetype200b8_D.lib"
# ADD LIB32 /nologo /out:"..\..\..\objs\freetype2110MT_D.lib"

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "freetype___Win32_Release_Multithreaded"
# PROP BASE Intermediate_Dir "freetype___Win32_Release_Multithreaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\objs\release_mt"
# PROP Intermediate_Dir "..\..\..\objs\release_mt"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Za /W3 /GX /O2 /I "..\freetype\include\\" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "FT_FLAT_COMPILE" /YX /FD /c
# ADD CPP /MT /Za /W4 /GX /Zi /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /nologo /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\freetype200b8.lib"
# ADD LIB32 /nologo /out:"..\..\..\objs\freetype2110MT.lib"

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Singlethreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "freetype___Win32_Release_Singlethreaded"
# PROP BASE Intermediate_Dir "freetype___Win32_Release_Singlethreaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\objs\release_st"
# PROP Intermediate_Dir "..\..\..\objs\release_st"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /Za /W4 /GX /Zi /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /Za /W4 /GX /Zi /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /nologo /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\objs\freetype206.lib"
# ADD LIB32 /out:"..\..\..\objs\freetype2110ST.lib"
# SUBTRACT LIB32 /nologo

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Singlethreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "freetype___Win32_Debug_Singlethreaded"
# PROP BASE Intermediate_Dir "freetype___Win32_Debug_Singlethreaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\objs\debug_st"
# PROP Intermediate_Dir "..\..\..\objs\debug_st"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /Za /W4 /Gm /GX /Zi /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "FT_DEBUG_LEVEL_ERROR" /D "FT_DEBUG_LEVEL_TRACE" /FD /GZ /c
# SUBTRACT BASE CPP /X /YX
# ADD CPP /Za /W4 /GX /Zi /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "FT_DEBUG_LEVEL_ERROR" /D "FT_DEBUG_LEVEL_TRACE" /FD /GZ /c
# SUBTRACT CPP /nologo /X /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\objs\freetype206_D.lib"
# ADD LIB32 /nologo /out:"..\..\..\objs\freetype2110ST_D.lib"

!ENDIF 

# Begin Target

# Name "freetype - Win32 Release"
# Name "freetype - Win32 Debug"
# Name "freetype - Win32 Debug Multithreaded"
# Name "freetype - Win32 Release Multithreaded"
# Name "freetype - Win32 Release Singlethreaded"
# Name "freetype - Win32 Debug Singlethreaded"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "bdf"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\bdf\bdf.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\bdf\bdfdrivr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\bdf\bdflib.c
# End Source File
# End Group
# Begin Group "cff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\cff\cff.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cff\cffcmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cff\cffdrivr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cff\cffgload.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cff\cffload.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cff\cffobjs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cff\cffparse.c
# End Source File
# End Group
# Begin Group "base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\base\ftapi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftbase.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftbbox.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftbdf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftbitmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftcalc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftdbgmem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftdebug.c

!IF  "$(CFG)" == "freetype - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Multithreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Multithreaded"

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Singlethreaded"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Singlethreaded"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftgloadr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftglyph.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftinit.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftmac.c

!IF  "$(CFG)" == "freetype - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Multithreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Multithreaded"

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Singlethreaded"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Singlethreaded"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftmm.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftnames.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftobjs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftotval.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftoutln.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftpfr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftrfork.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftstream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftstroke.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftsynth.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftsystem.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\fttrigon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\fttype1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftutil.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftwinfnt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\base\ftxf86.c
# End Source File
# End Group
# Begin Group "cache"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\cache\ftcache.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cache\ftcbasic.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cache\ftccache.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cache\ftccmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cache\ftcglyph.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cache\ftcimage.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cache\ftcmanag.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cache\ftcmru.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cache\ftcsbits.c
# End Source File
# End Group
# Begin Group "gzip"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\gzip\adler32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gzip\ftgzip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gzip\infblock.c

!IF  "$(CFG)" == "freetype - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Multithreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Multithreaded"

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Singlethreaded"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Singlethreaded"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gzip\infcodes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gzip\inflate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gzip\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gzip\infutil.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gzip\zutil.c
# End Source File
# End Group
# Begin Group "lzw"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\lzw\ftlzw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lzw\zopen.c
# End Source File
# End Group
# Begin Group "pcf"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\pcf\pcf.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pcf\pcfdrivr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pcf\pcfread.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pcf\pcfutil.c
# End Source File
# End Group
# Begin Group "pfr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\pfr\pfr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pfr\pfrcmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pfr\pfrdrivr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pfr\pfrgload.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pfr\pfrload.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pfr\pfrobjs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pfr\pfrsbit.c
# End Source File
# End Group
# Begin Group "psaux"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\psaux\psaux.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\psaux\psauxmod.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\psaux\psobjs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\psaux\t1cmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\psaux\t1decode.c
# End Source File
# End Group
# Begin Group "pshinter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\pshinter\pshalgo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pshinter\pshglob.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pshinter\pshinter.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pshinter\pshmod.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pshinter\pshrec.c
# End Source File
# End Group
# Begin Group "psnames"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\psnames\psmodule.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\psnames\psnames.c
# End Source File
# End Group
# Begin Group "raster"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\raster\ftraster.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\raster\ftrend1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\raster\raster.c
# SUBTRACT CPP /Fr
# End Source File
# End Group
# Begin Group "sfnt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\sfnt\sfdriver.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sfnt\sfnt.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sfnt\sfobjs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sfnt\ttcmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sfnt\ttkern.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sfnt\ttload.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sfnt\ttpost.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sfnt\ttsbit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sfnt\ttsbit0.c
# End Source File
# End Group
# Begin Group "smooth"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\smooth\ftgrays.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\smooth\ftsmooth.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\smooth\smooth.c
# SUBTRACT CPP /Fr
# End Source File
# End Group
# Begin Group "truetype"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\truetype\truetype.c
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=..\..\..\src\truetype\ttdriver.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\truetype\ttgload.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\truetype\ttgxvar.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\truetype\ttinterp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\truetype\ttobjs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\truetype\ttpload.c
# End Source File
# End Group
# Begin Group "type1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\type1\t1afm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\type1\t1driver.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\type1\t1gload.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\type1\t1load.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\type1\t1objs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\type1\t1parse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\type1\type1.c
# SUBTRACT CPP /Fr
# End Source File
# End Group
# Begin Group "cid"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\cid\cidgload.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cid\cidload.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cid\cidobjs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cid\cidparse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cid\cidriver.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cid\type1cid.c
# SUBTRACT CPP /Fr
# End Source File
# End Group
# Begin Group "type42"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\type42\type42.c
# SUBTRACT CPP /Fr
# End Source File
# End Group
# Begin Group "winfonts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\winfonts\winfnt.c
# SUBTRACT CPP /Fr
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\autohint\autohint.c

!IF  "$(CFG)" == "freetype - Win32 Release"

# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug"

# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Multithreaded"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Multithreaded"

# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Singlethreaded"

# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Singlethreaded"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /Fr

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ftdebug.c

!IF  "$(CFG)" == "freetype - Win32 Release"

# ADD CPP /Ze
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug"

# ADD CPP /Ze
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Multithreaded"

# ADD CPP /Ze
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Multithreaded"

# ADD CPP /Ze
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "freetype - Win32 Release Singlethreaded"

# ADD CPP /Ze
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug Singlethreaded"

# PROP Exclude_From_Build 1
# ADD CPP /Ze
# SUBTRACT CPP /Fr

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\ft2build.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\freetype\config\ftconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\freetype\config\ftheader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\freetype\config\ftmodule.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\freetype\config\ftoption.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\freetype\config\ftstdlib.h
# End Source File
# End Group
# End Target
# End Project
