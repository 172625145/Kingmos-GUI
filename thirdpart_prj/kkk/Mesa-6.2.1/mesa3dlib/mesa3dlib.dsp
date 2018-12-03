# Microsoft Developer Studio Project File - Name="mesa3dlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mesa3dlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mesa3dlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mesa3dlib.mak" CFG="mesa3dlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mesa3dlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mesa3dlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../include" /I "../src/mesa/main" /I "../src/mesa/glapi" /I "../src/mesa" /I "../src/mesa/shader" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "GLUT_BUILDING_LIB" /D "MESA" /FR /YX /FD /GZ /c
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

# Name "mesa3dlib - Win32 Release"
# Name "mesa3dlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "array_cache"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mesa\array_cache\ac_context.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\array_cache\ac_import.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "gdi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mesa\drivers\common\driverfuncs.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\drivers\windows\gdi\wgl.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\drivers\windows\gdi\wmesa.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "glapi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mesa\glapi\glapi.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\glapi\glthread.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "glu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\arc.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\arcsorter.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\arctess.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\backend.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\basiccrveval.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\basicsurfeval.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\interface\bezierEval.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\interface\bezierPatch.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\interface\bezierPatchMesh.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\bin.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\bufpool.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\cachingeval.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\ccw.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\coveandtiler.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\curve.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\curvelist.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\curvesub.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\dataTransform.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libtess\dict.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\directedLine.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\displaylist.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libutil\error.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\flist.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\flistsorter.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libtess\geom.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\interface\glcurveval.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\interface\glinterface.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\interface\glrenderer.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\interface\glsurfeval.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\glu.def

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libutil\glue.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\gridWrap.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\hull.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\interface\incurveeval.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\interface\insurfeval.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\intersect.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\knotvector.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\mapdesc.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\mapdescv.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\maplist.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libtess\memalloc.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libtess\mesh.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\mesher.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libutil\mipmap.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\monoChain.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\monoPolyPart.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\monotonizer.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\monoTriangulation.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\monoTriangulationBackend.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\mycode.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libtess\normal.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\nurbsinterfac.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\nurbstess.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\partitionX.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\partitionY.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\patch.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\patchlist.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\polyDBG.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\polyUtil.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\primitiveStream.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libtess\priorityq.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libutil\project.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libutil\quad.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\quicksort.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\quilt.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\reader.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\rectBlock.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libutil\registry.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libtess\render.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\renderhints.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\sampleComp.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\sampleCompBot.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\sampleCompRight.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\sampleCompTop.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\sampledLine.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\sampleMonoPoly.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\nurbtess\searchTree.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\slicer.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\sorter.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\splitarcs.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\subdivider.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libtess\sweep.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libtess\tess.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libtess\tessmono.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\tobezier.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\trimline.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\trimregion.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\trimvertpool.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\uarray.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\glu\sgi\libnurbs\internals\varray.cc

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mesa\main\accum.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\api_arrayelt.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\api_loopback.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\api_noop.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\api_validate.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\attrib.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\blend.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\bufferobj.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\buffers.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\clip.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\colortab.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\context.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\convolve.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\debug.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\depth.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\dispatch.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\dlist.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\drawpix.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\enable.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\enums.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\eval.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\extensions.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\feedback.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\fog.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\get.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\hash.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\hint.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\histogram.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\image.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\imports.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\light.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\lines.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\matrix.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\occlude.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\pixel.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\points.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\polygon.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\rastpos.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\state.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\stencil.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\texcompress.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\texcompress_fxt1.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\texcompress_s3tc.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\texformat.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\teximage.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\texobj.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\texstate.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\texstore.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\varray.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\main\vtxfmt.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mesa\math\m_debug_clip.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\math\m_debug_norm.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\math\m_debug_xform.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\math\m_eval.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\math\m_matrix.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\math\m_translate.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\math\m_vector.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\math\m_xform.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "osmesa"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mesa\drivers\osmesa\osmesa.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\drivers\osmesa\osmesa.def

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "shader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mesa\shader\arbfragparse.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\shader\arbprogparse.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\shader\arbprogram.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\shader\arbvertparse.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\shader\grammar.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\src\mesa\shader\grammar_mesa.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\shader\nvfragparse.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\shader\nvprogram.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\shader\nvvertexec.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\shader\nvvertparse.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\shader\program.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "swrast"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mesa\swrast\s_aaline.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_aatriangle.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_accum.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_alpha.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_alphabuf.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_auxbuffer.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_bitmap.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_blend.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_buffers.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_context.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_copypix.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_depth.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_drawpix.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_feedback.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_fog.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_imaging.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_lines.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_logic.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_masking.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_nvfragprog.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_pixeltex.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_points.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_readpix.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_span.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_stencil.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_texstore.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_texture.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_triangle.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast\s_zoom.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "swrast_setup"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mesa\swrast_setup\ss_context.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\swrast_setup\ss_triangle.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "tnl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mesa\tnl\t_array_api.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_array_import.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_context.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_pipeline.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_save_api.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_save_loopback.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_save_playback.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vb_cull.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vb_fog.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vb_light.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vb_normals.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vb_points.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vb_program.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vb_render.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vb_texgen.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vb_texmat.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vb_vertex.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vertex.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vertex_c.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vertex_codegen.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vtx_api.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vtx_eval.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vtx_exec.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mesa\tnl\t_vtx_generic.c

!IF  "$(CFG)" == "mesa3dlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mesa3dlib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
