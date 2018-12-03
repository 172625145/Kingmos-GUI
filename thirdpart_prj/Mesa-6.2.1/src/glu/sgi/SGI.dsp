# Microsoft Developer Studio Project File - Name="SGI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SGI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SGI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SGI.mak" CFG="SGI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SGI - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SGI - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SGI - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "../../../../../include" /I "../../../include" /I "./include" /I "./libnurbs/nurbtess" /I "./libnurbs/internals" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "VC386" /D "WINDOWS_KERNEL" /D "LIBRARYBUILD" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SGI - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../../../include" /I "../../../include" /I "./include" /I "./libnurbs/nurbtess" /I "./libnurbs/internals" /D "_WINDOWS" /D "_USRDLL" /D "GLU_EXPORTS" /D "BUILD_GL32" /D "_DEBUG" /D "VC386" /D "LIBRARYBUILD" /D "WIN32" /D "_MBCS" /D "WINDOWS_KERNEL" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\GLU32.LIB"

!ENDIF 

# Begin Target

# Name "SGI - Win32 Release"
# Name "SGI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\libnurbs\internals\arc.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\arcsorter.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\arctess.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\backend.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\basiccrveval.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\basicsurfeval.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\interface\bezierEval.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\interface\bezierPatch.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\interface\bezierPatchMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\bin.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\bufpool.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\cachingeval.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\ccw.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\coveandtiler.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\curve.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\curvelist.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\curvesub.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\dataTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\libtess\dict.c
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\directedLine.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\displaylist.cpp
# End Source File
# Begin Source File

SOURCE=.\libutil\error.c
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\flist.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\flistsorter.cpp
# End Source File
# Begin Source File

SOURCE=.\libtess\geom.c
# End Source File
# Begin Source File

SOURCE=.\libnurbs\interface\glcurveval.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\interface\glinterface.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\interface\glrenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\interface\glsurfeval.cpp
# End Source File
# Begin Source File

SOURCE=..\mesa\glu.c
# End Source File
# Begin Source File

SOURCE=.\glu.def
# End Source File
# Begin Source File

SOURCE=.\libutil\glue.c
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\gridWrap.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\hull.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\interface\incurveeval.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\interface\insurfeval.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\intersect.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\knotvector.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\mapdesc.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\mapdescv.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\maplist.cpp
# End Source File
# Begin Source File

SOURCE=.\libtess\memalloc.c
# End Source File
# Begin Source File

SOURCE=.\libtess\mesh.c
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\mesher.cpp
# End Source File
# Begin Source File

SOURCE=.\libutil\mipmap.c
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\monoChain.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\monoPolyPart.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\monotonizer.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\monoTriangulation.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\monoTriangulationBackend.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\mycode.cpp
# End Source File
# Begin Source File

SOURCE=.\libtess\normal.c
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\nurbsinterfac.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\nurbstess.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\partitionX.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\partitionY.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\patch.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\patchlist.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\polyDBG.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\polyUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\primitiveStream.cpp
# End Source File
# Begin Source File

SOURCE=.\libtess\priorityq.c
# End Source File
# Begin Source File

SOURCE=.\libutil\project.c
# End Source File
# Begin Source File

SOURCE=.\libutil\quad.c
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\quicksort.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\quilt.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\reader.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\rectBlock.cpp
# End Source File
# Begin Source File

SOURCE=.\libutil\registry.c
# End Source File
# Begin Source File

SOURCE=.\libtess\render.c
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\renderhints.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\sampleComp.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\sampleCompBot.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\sampleCompRight.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\sampleCompTop.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\sampledLine.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\sampleMonoPoly.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\nurbtess\searchTree.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\slicer.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\sorter.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\splitarcs.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\subdivider.cpp
# End Source File
# Begin Source File

SOURCE=.\libtess\sweep.c
# End Source File
# Begin Source File

SOURCE=.\libtess\tess.c
# End Source File
# Begin Source File

SOURCE=.\libtess\tessmono.c
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\tobezier.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\trimline.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\trimregion.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\trimvertpool.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\uarray.cpp
# End Source File
# Begin Source File

SOURCE=.\libnurbs\internals\varray.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
