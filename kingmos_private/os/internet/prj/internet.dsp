# Microsoft Developer Studio Project File - Name="internet" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=internet - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "internet.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "internet.mak" CFG="internet - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "internet - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "internet - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "internet - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(KINGMOS_ROOT)\kingmos_private\os\internet\inc_app" /I "$(KINGMOS_ROOT)\kingmos_private\os\internet\inc" /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\include" /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\arch\i386-win\include" /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\drivers\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "VC386" /D "KINGMOS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "internet - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(KINGMOS_ROOT)\kingmos_private\os\internet\inc_app" /I "$(KINGMOS_ROOT)\kingmos_private\os\internet\inc" /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\include" /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\arch\i386-win\include" /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\drivers\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "VC386" /D "KINGMOS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "internet - Win32 Release"
# Name "internet - Win32 Debug"
# Begin Group "internet"

# PROP Default_Filter ""
# Begin Group "http"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\http\http.c
# End Source File
# End Group
# Begin Group "ftp"

# PROP Default_Filter ""
# End Group
# Begin Group "inet"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\inet\inet.c
# End Source File
# Begin Source File

SOURCE=..\inet\inet_regapi.c
# End Source File
# End Group
# Begin Group "inetstr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\inetstr\inetstr.c
# End Source File
# End Group
# Begin Group "inetapi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\inetapi\inet_callapi.c
# End Source File
# End Group
# End Group
# Begin Group "inc_app"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\netsrv\inc_app\einternet.h
# End Source File
# Begin Source File

SOURCE=..\inc_app\inetstr.h
# End Source File
# Begin Source File

SOURCE=..\..\netsrv\inc_app\internet.h
# End Source File
# End Group
# Begin Group "inc_local"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\inc\encode.h
# End Source File
# Begin Source File

SOURCE=..\inc\inet_call.h
# End Source File
# Begin Source File

SOURCE=..\inc\inet_def.h
# End Source File
# Begin Source File

SOURCE=..\inc\skt_call.h
# End Source File
# Begin Source File

SOURCE=..\inc\sktcall_ver.h
# End Source File
# End Group
# End Target
# End Project
