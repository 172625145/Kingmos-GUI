# Microsoft Developer Studio Project File - Name="win32core" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=win32core - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "win32core.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "win32core.mak" CFG="win32core - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "win32core - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "win32core - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "win32core - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\include" /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\arch\i386-win\include" /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\drivers\include" /I "$(KINGMOS_ROOT)\kingmos_private\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "KINGMOS" /D "VC386" /D "ZT_PHONE" /FR /YX /FD /Zm500 /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "win32core - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\include" /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\arch\i386-win\include" /I "$(KINGMOS_ROOT)\$(KINGMOS_PROJECT)\drivers\include" /I "$(KINGMOS_ROOT)\kingmos_private\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "KINGMOS" /D "VC386" /D "ZT_PHONE" /FR /YX /FD /GZ /Zm500 /c
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

# Name "win32core - Win32 Release"
# Name "win32core - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "apicall"

# PROP Default_Filter ""
# Begin Group "apisrv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\apisrv\apisrv\apilib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\caretlib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\classlib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\corelib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\devlib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\dlglib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\filelib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\fsmgrlib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\gdilib.c
# End Source File
# Begin Source File

SOURCE=..\..\os\sms\api\gsmapis_call.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\keybdllib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\menulib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\msglib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\reglib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\rgnlib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\shelllib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\sysetlib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\tablelib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\usuallib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\wavelib.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\apisrv\winlib.c
# End Source File
# End Group
# Begin Group "unicode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\apisrv\unicode\gb2312d.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\unicode\unicode.c
# End Source File
# Begin Source File

SOURCE=..\..\apisrv\unicode\utf8.c
# End Source File
# End Group
# End Group
# Begin Group "os"

# PROP Default_Filter ""
# Begin Group "audio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\audio\APICall.c
# End Source File
# Begin Source File

SOURCE=..\..\os\audio\audio.c
# End Source File
# Begin Source File

SOURCE=..\..\os\audio\midi_api.c
# End Source File
# Begin Source File

SOURCE=..\..\os\audio\wavesrv.c
# End Source File
# End Group
# Begin Group "core"

# PROP Default_Filter ""
# Begin Group "kernel_core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\core\kernel\alarmmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\apisrv.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\blkheap.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\coresrv.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\downup.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\hmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\intrapi.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\isr.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\kalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\kassert.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\klheap.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\klmisc.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\ksysmem.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\pagemgr.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\process.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\program.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\qemodule.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\sche.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\semaphor.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\sleep.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\start.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\system.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\thread.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\tick.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\kernel\virtual.c

!IF  "$(CFG)" == "win32core - Win32 Release"

!ELSEIF  "$(CFG)" == "win32core - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "win32_core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\core\win32\emlcpu.c
# End Source File
# Begin Source File

SOURCE="..\..\..\kms_phone\arch\i386-win\kernel\emlsocket.c"
# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\inittss.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\intrlock.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\irq.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\mmu.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\oeminit.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\oemintr.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\oemtime.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\switch.asm

!IF  "$(CFG)" == "win32core - Win32 Release"

# Begin Custom Build
InputDir=\kingmos\kingmos_private\os\core\win32
IntDir=.\Release
InputPath=..\..\os\core\win32\switch.asm
InputName=switch

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(KINGMOS_ROOT)\masm\ml.exe -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ELSEIF  "$(CFG)" == "win32core - Win32 Debug"

# Begin Custom Build
InputDir=\kingmos\kingmos_private\os\core\win32
IntDir=.\Debug
InputPath=..\..\os\core\win32\switch.asm
InputName=switch

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(KINGMOS_ROOT)\masm\ml.exe -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\switchto.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\sysintr.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\try.c
# End Source File
# Begin Source File

SOURCE=..\..\os\core\win32\virmem.c
# End Source File
# End Group
# End Group
# Begin Group "device"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\device\device.c
# End Source File
# Begin Source File

SOURCE=..\..\os\device\devsrv.c
# End Source File
# End Group
# Begin Group "filesys"

# PROP Default_Filter ""
# Begin Group "kmfs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\filesys\kmfs\kmfs.c
# End Source File
# End Group
# Begin Group "fatfs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\filesys\fatfs\fatapi.c
# End Source File
# Begin Source File

SOURCE=..\..\os\filesys\fatfs\fatdisk.c
# End Source File
# Begin Source File

SOURCE=..\..\os\filesys\fatfs\file.c
# End Source File
# Begin Source File

SOURCE=..\..\os\filesys\fatfs\find.c
# End Source File
# Begin Source File

SOURCE=..\..\os\filesys\fatfs\load.c
# End Source File
# Begin Source File

SOURCE=..\..\os\filesys\fatfs\misc.c
# End Source File
# Begin Source File

SOURCE=..\..\os\filesys\fatfs\mount.c
# End Source File
# Begin Source File

SOURCE=..\..\os\filesys\fatfs\new.c
# End Source File
# End Group
# Begin Group "romfs"

# PROP Default_Filter ""
# End Group
# Begin Group "fsmain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\filesys\fsmain\filesrv.c
# End Source File
# Begin Source File

SOURCE=..\..\os\filesys\fsmain\fsdmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\os\filesys\fsmain\fsmain.c
# End Source File
# End Group
# Begin Group "registry"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\filesys\registry\reg.c
# End Source File
# End Group
# End Group
# Begin Group "gwme_drv"

# PROP Default_Filter ""
# Begin Group "gdidrv"

# PROP Default_Filter ""
# Begin Group "32bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\32bpp\gdidrv32b.c
# End Source File
# End Group
# Begin Group "16bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\16bpp\gdidrv16b.c
# End Source File
# End Group
# Begin Group "8bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\8bpp\gdidrv8b.c
# End Source File
# End Group
# Begin Group "1bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\1bpp\gdidrv1b.c
# End Source File
# End Group
# Begin Group "sysfont"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\sysfont\efont8x6.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\sysfont\efont8x8.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\sysfont\engcode.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\sysfont\phonetic.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\sysfont\symfont.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\sysfont\sysfont16.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\sysfont\sysfont24.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\sysfont\ttffont.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "4bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\4bpp\gdidrv4b.c
# End Source File
# End Group
# Begin Group "24bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_drv\gdidrv\24bpp\gdidrv24b.c
# End Source File
# End Group
# End Group
# Begin Group "gwme"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\calibrat.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\caret.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\class.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\gdi.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\gwme.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\gwmesrv.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\imageobj.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\keydrv.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\largenum.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\loadimg.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\message.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\msgqueue.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\paintmsg.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\posdrv.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\rgn.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\stockobj.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\sysset.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\timer.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\win.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\winfont.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_drv\gwme\wintimer.c
# End Source File
# End Group
# End Group
# Begin Group "http_win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\http_win32\digest.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\os\http_win32\encode.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\os\http_win32\internetaccess.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\os\http_win32\md5.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "dict"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\ecdictlib\dictsrch.c
# End Source File
# End Group
# Begin Group "usualapi"

# PROP Default_Filter ""
# Begin Group "clipboard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\usualapi\clipboard\clpboard.c
# End Source File
# End Group
# Begin Group "globmem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\usualapi\globmem\globmem.c
# End Source File
# End Group
# Begin Group "usualsrv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\usualapi\usualsrv\usualsrv.c
# End Source File
# End Group
# End Group
# Begin Group "table"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\table\table.c
# End Source File
# Begin Source File

SOURCE=..\..\os\table\tablesrv.c
# End Source File
# End Group
# Begin Group "gwme_gui"

# PROP Default_Filter ""
# Begin Group "defproc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_gui\defproc\defproc.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\defproc\dialog.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\defproc\msgbox.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\defproc\stockstr.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\defproc\touchcal.c
# End Source File
# End Group
# Begin Group "wndclass"

# PROP Default_Filter ""
# Begin Group "sysclass"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\sysclass\sysclass.c
# End Source File
# End Group
# Begin Group "static"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\static\static.c
# End Source File
# End Group
# Begin Group "button"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\button\button.c
# End Source File
# End Group
# Begin Group "combobox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\combobox\combbox.c
# End Source File
# End Group
# Begin Group "edit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\edit\edit.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\edit\editbase.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\edit\multiple.c
# End Source File
# End Group
# Begin Group "listctrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\listctrl\listctrl.c
# End Source File
# End Group
# Begin Group "menu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\menu\menu.c
# End Source File
# End Group
# Begin Group "list"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\list\list.c
# End Source File
# End Group
# Begin Group "scrollbar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\scrollbar\scrlbar.c
# End Source File
# End Group
# Begin Group "smallie"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\smallie\control.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\smallie\download.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\smallie\htmtable.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\smallie\locate.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\smallie\showimg.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\smallie\sietable.c
# End Source File
# Begin Source File

SOURCE=..\..\os\gwme_gui\wndclass\smallie\sieview.c
# End Source File
# End Group
# End Group
# End Group
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
