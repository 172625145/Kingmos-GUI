# Microsoft Developer Studio Project File - Name="gdi_lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gdi_lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gdi_lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gdi_lib.mak" CFG="gdi_lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gdi_lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gdi_lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gdi_lib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../../../../src/mesa/main" /I "../../../../../src/mesa/glapi" /I "../../../../../src/mesa/math" /I "../../../../../src/mesa/transform" /I "../../../../../src/mesa/swrast" /I "../../../../../src/mesa/swrast_setup" /I "../../../../../src/mesa" /I "../../../../../src/mesa/shader" /I "../../../../../../../include" /I "../../../../../include" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "WINDOWS_KERNEL" /D "VC386" /D "GDI_EXPORTS" /D "BUILD_GL32" /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\OPENGL32.LIB"

!ELSEIF  "$(CFG)" == "gdi_lib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../../../src/mesa/main" /I "../../../../../src/mesa/glapi" /I "../../../../../src/mesa/math" /I "../../../../../src/mesa/transform" /I "../../../../../src/mesa/swrast" /I "../../../../../src/mesa/swrast_setup" /I "../../../../../src/mesa" /I "../../../../../src/mesa/shader" /I "../../../../../../../include" /I "../../../../../include" /D "_WINDOWS" /D "_USRDLL" /D "GDI_EXPORTS" /D "BUILD_GL32" /D "_DEBUG" /D "VC386" /D "WIN32" /D "_MBCS" /D "WINDOWS_KERNEL" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\OPENGL32.LIB"

!ENDIF 

# Begin Target

# Name "gdi_lib - Win32 Release"
# Name "gdi_lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\common\driverfuncs.c
# End Source File
# Begin Source File

SOURCE=.\error.c
# End Source File
# Begin Source File

SOURCE=.\glue.c
# End Source File
# Begin Source File

SOURCE=.\mesa.def
# End Source File
# Begin Source File

SOURCE=.\quad.c
# End Source File
# Begin Source File

SOURCE=.\wgl.c
# End Source File
# Begin Source File

SOURCE=.\wmesa.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
