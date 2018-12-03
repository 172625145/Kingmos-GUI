# Microsoft Developer Studio Project File - Name="kingmos_gui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=kingmos_gui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kingmos_gui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kingmos_gui.mak" CFG="kingmos_gui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kingmos_gui - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "kingmos_gui - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Kingmos_gwme_linux/build/kingmos_gui", BDAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kingmos_gui - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\arch\i386-win-kernel\include" /I "..\..\drivers\include" /I "..\..\kingmos_private\include" /I "..\..\kingmos_private\os\wapsrv\inc" /I "..\..\kingmos_private\os\netsrv\inc" /I "..\..\kingmos_private\os\internet\inc" /I "..\..\thirdpart_prj\freetype-2.1.10\include" /I "..\..\thirdpart_prj\Mesa-6.2.1_app\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_WINDOWS" /D "VC386" /D "KINGMOS" /D "WINDOWS_KERNEL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "kingmos_gui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /I "..\..\include" /I "..\..\arch\i386-win-kernel\include" /I "..\..\drivers\include" /I "..\..\kingmos_private\include" /I "..\..\kingmos_private\os\wapsrv\inc" /I "..\..\kingmos_private\os\netsrv\inc" /I "..\..\kingmos_private\os\internet\inc" /I "..\..\thirdpart_prj\freetype-2.1.10\include" /I "..\..\thirdpart_prj\Mesa-6.2.1_app\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "VC386" /D "_WINDOWS" /D "KINGMOS" /D "WINDOWS_KERNEL" /D "CANON_DIGCMP" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Winmm.lib Ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "kingmos_gui - Win32 Release"
# Name "kingmos_gui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Application"

# PROP Default_Filter ""
# Begin Group "apmain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\application\apmain\apmain.c
# End Source File
# End Group
# Begin Group "demos"

# PROP Default_Filter ""
# Begin Group "demo_alpha"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\application\demos\demo_alpha\alpha_window.c
# End Source File
# End Group
# Begin Group "demo_font"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\application\demos\demo_font\font_window.c
# End Source File
# End Group
# Begin Group "demo_layer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\application\demos\demo_layer\layer_window.c
# End Source File
# End Group
# Begin Group "demo_normal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\application\demos\demo_normal\normal_window.c
# End Source File
# End Group
# Begin Group "demo_special"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\application\demos\demo_special\rgn_window.c
# End Source File
# End Group
# Begin Group "opengl"

# PROP Default_Filter ""
# Begin Group "gears"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\application\opengl\gears\gears.c
# End Source File
# End Group
# Begin Group "stex3d"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\application\opengl\stex3d\stex3d.c
# End Source File
# End Group
# End Group
# Begin Group "demo_gps"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\application\demos\demo_gps\gps_window.c
# End Source File
# End Group
# Begin Group "demo_gra"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\application\demogradfill\polygon.c
# End Source File
# Begin Source File

SOURCE=..\..\application\demogradfill\testgradfill.c
# End Source File
# End Group
# End Group
# Begin Group "test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\application\test\test.c
# End Source File
# End Group
# End Group
# Begin Group "arch"

# PROP Default_Filter ""
# Begin Group "i386-win32-kernel"

# PROP Default_Filter ""
# Begin Group "kernel"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\app.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\core.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\cri.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\emlcpu.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\emldsk.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\filesys.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\imp.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\initkernel.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\interlock.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\irq.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\mem.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\process.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\server.c"
# End Source File
# End Group
# Begin Group "drivers_pdd"

# PROP Default_Filter ""
# Begin Group "mouse"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\drivers\mouse\mouse.c"
# End Source File
# End Group
# Begin Group "display"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\drivers\display16bpp\w32cfg.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\drivers\display16bpp\windisp.c"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\drivers\display16bpp\windrv.c"
# End Source File
# End Group
# Begin Group "sysgwdi"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\drivers\sysgwdi\sysgwdi.c"
# End Source File
# End Group
# Begin Group "keybrd"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\drivers\keybrd\keybrd.c"
# End Source File
# End Group
# End Group
# Begin Group "boot"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\boot\boot.c"
# End Source File
# End Group
# End Group
# End Group
# Begin Group "component"

# PROP Default_Filter ""
# Begin Group "desktop"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\component\desktop\desktop.c
# End Source File
# Begin Source File

SOURCE=..\..\component\desktop\statebar.c
# End Source File
# End Group
# Begin Group "keyboard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\component\keyboard\engkbd.c
# End Source File
# Begin Source File

SOURCE=..\..\component\keyboard\keybdsrv.c
# End Source File
# Begin Source File

SOURCE=..\..\component\keyboard\keyboard.c
# End Source File
# Begin Source File

SOURCE=..\..\component\keyboard\keyvirtualtochar.c
# End Source File
# Begin Source File

SOURCE=..\..\component\keyboard\lxsearch.c
# End Source File
# Begin Source File

SOURCE=..\..\component\keyboard\pykbd.c
# End Source File
# Begin Source File

SOURCE=..\..\component\keyboard\pysearch.c
# End Source File
# Begin Source File

SOURCE=..\..\component\keyboard\pytab.c
# End Source File
# Begin Source File

SOURCE=..\..\component\keyboard\spellwnd.c
# End Source File
# End Group
# Begin Group "shell"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\component\shell\eshell.c
# End Source File
# Begin Source File

SOURCE=..\..\component\shell\shellsrv.c
# End Source File
# End Group
# Begin Group "commdlg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\component\commdlg\ecommdlg.c
# End Source File
# End Group
# End Group
# Begin Group "drivers"

# PROP Default_Filter ""
# Begin Group "fontcfg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\drivers\fontcfg\sysfont.c
# End Source File
# End Group
# End Group
# Begin Group "private"

# PROP Default_Filter ""
# Begin Group "apicall"

# PROP Default_Filter ""
# Begin Group "apisrv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\apilib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\caretlib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\classlib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\dlglib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\gdilib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\keybdllib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\menulib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\msglib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\reglib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\rgnlib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\shelllib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\sysetlib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\tablelib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\usuallib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\wavelib.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\apisrv\winlib.c
# End Source File
# End Group
# Begin Group "unicode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\unicode\1252.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\unicode\gb2312d.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\unicode\unicode.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\apisrv\unicode\utf8.c
# End Source File
# End Group
# End Group
# Begin Group "os"

# PROP Default_Filter ""
# Begin Group "gwme_drv"

# PROP Default_Filter ""
# Begin Group "gdidrv"

# PROP Default_Filter ""
# Begin Group "32bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\32bpp\gdidrv32b.c
# End Source File
# End Group
# Begin Group "16bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\16bpp\gdidrv16b.c
# End Source File
# End Group
# Begin Group "8bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\8bpp\gdidrv8b.c
# End Source File
# End Group
# Begin Group "1bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\1bpp\gdidrv1b.c
# End Source File
# End Group
# Begin Group "sysfont"

# PROP Default_Filter ""
# Begin Group "jpcodec"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\jpcodec\qeucjpcodec.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\jpcodec\qjiscodec.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\jpcodec\qjpunicode.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\jpcodec\qsjiscodec.c
# End Source File
# End Group
# Begin Group "bigcodec"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\big5codec\qbig5codec.c
# End Source File
# End Group
# Begin Group "korea"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\korea\qeuckrcodec.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\sysfont_main\936code_new.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\sysfont_main\efont8x6.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\sysfont_main\efont8x8.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\sysfont_main\engcode.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\sysfont_main\kms_freetype_2_1_10.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\sysfont_main\phonetic.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\sysfont_main\symfont.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\sysfont_main\sysfont16.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\sysfont\sysfont_main\sysfont24.c
# End Source File
# End Group
# Begin Group "4bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\4bpp\gdidrv4b.c
# End Source File
# End Group
# Begin Group "24bpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gdidrv\24bpp\gdidrv24b.c
# End Source File
# End Group
# End Group
# Begin Group "gwme"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\calibrat.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\caret.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\class.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\gdi.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\gwme.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\gwmesrv.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\gwmetask.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\imageobj.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\keydrv.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\largenum.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\layergdi.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\layerwin.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\loadimg.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\message.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\msgqueue.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\paintmsg.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\posdrv.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\rgn.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\stockobj.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\sysset.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\timer.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\vdriver.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\win.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\winfont.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_drv\gwme\wintimer.c
# End Source File
# End Group
# End Group
# Begin Group "usualapi"

# PROP Default_Filter ""
# Begin Group "clipboard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\usualapi\clipboard\clpboard.c
# End Source File
# End Group
# Begin Group "globmem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\usualapi\globmem\globmem.c
# End Source File
# End Group
# Begin Group "usualsrv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\usualapi\usualsrv\usualsrv.c
# End Source File
# End Group
# End Group
# Begin Group "gwme_gui"

# PROP Default_Filter ""
# Begin Group "defproc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\defproc\defproc.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\defproc\dialog.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\defproc\msgbox.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\defproc\roundrgn.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\defproc\stockstr.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\defproc\touchcal.c
# End Source File
# End Group
# Begin Group "wndclass"

# PROP Default_Filter ""
# Begin Group "sysclass"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\wndclass\sysclass\sysclass.c
# End Source File
# End Group
# Begin Group "static"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\wndclass\static\static.c
# End Source File
# End Group
# Begin Group "button"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\wndclass\button\button.c
# End Source File
# End Group
# Begin Group "combobox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\wndclass\combobox\combbox.c
# End Source File
# End Group
# Begin Group "edit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\wndclass\edit\edit.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\wndclass\edit\editbase.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\wndclass\edit\multiple.c
# End Source File
# End Group
# Begin Group "listctrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\wndclass\listctrl\listctrl.c
# End Source File
# End Group
# Begin Group "menu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\wndclass\menu\menu.c
# End Source File
# End Group
# Begin Group "list"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\wndclass\list\list.c
# End Source File
# End Group
# Begin Group "scrollbar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\gwme_gui\wndclass\scrollbar\scrlbar.c
# End Source File
# End Group
# End Group
# End Group
# End Group
# Begin Group "filesys"

# PROP Default_Filter ""
# Begin Group "registry"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\kingmos_private\os\filesys\registry\reg.c
# End Source File
# Begin Source File

SOURCE=..\..\kingmos_private\os\filesys\registry\regsrv.c
# End Source File
# End Group
# End Group
# End Group
# Begin Group "library"

# PROP Default_Filter ""
# Begin Group "stdlib"

# PROP Default_Filter ""
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\stdlib\common\_sprintf.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\assert.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\bheap_new.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\dbgalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\dbgcs.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\ectype.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\filepath.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\fncmp.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\objlist.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\ptrlist.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\rect.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\stdlib.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\time.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\common\ttime.c
# End Source File
# End Group
# Begin Group "i386"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\stdlib\i386\longjmp.asm

!IF  "$(CFG)" == "kingmos_gui - Win32 Release"

# Begin Custom Build
InputDir=\kingmos\Kingmos_gui\library\stdlib\i386
IntDir=.\Release
InputPath=..\..\library\stdlib\i386\longjmp.asm
InputName=longjmp

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\masm\ml.exe -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ELSEIF  "$(CFG)" == "kingmos_gui - Win32 Debug"

# Begin Custom Build
InputDir=\kingmos\Kingmos_gui\library\stdlib\i386
IntDir=.\Debug
InputPath=..\..\library\stdlib\i386\longjmp.asm
InputName=longjmp

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\masm\ml.exe -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\i386\setjmp.asm

!IF  "$(CFG)" == "kingmos_gui - Win32 Release"

# Begin Custom Build
InputDir=\kingmos\Kingmos_gui\library\stdlib\i386
IntDir=.\Release
InputPath=..\..\library\stdlib\i386\setjmp.asm
InputName=setjmp

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\masm\ml.exe -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ELSEIF  "$(CFG)" == "kingmos_gui - Win32 Debug"

# Begin Custom Build
InputDir=\kingmos\Kingmos_gui\library\stdlib\i386
IntDir=.\Debug
InputPath=..\..\library\stdlib\i386\setjmp.asm
InputName=setjmp

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\masm\ml.exe -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\i386\string.c
# End Source File
# Begin Source File

SOURCE=..\..\library\stdlib\i386\wstring.c
# End Source File
# End Group
# End Group
# Begin Group "userlib"

# PROP Default_Filter ""
# Begin Group "classdef"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\classdef\strclass.c
# End Source File
# End Group
# Begin Group "universe"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\universe\imglist.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\universe\playsnd.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\universe\stracmp.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\universe\universe.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\universe\ymdlunar.c
# End Source File
# End Group
# Begin Group "gif"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\gif\dgif_lib.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\gif\gif2bmp.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\gif\gif_err.c
# End Source File
# End Group
# Begin Group "commctrl"

# PROP Default_Filter ""
# Begin Group "toolbar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\tbbutton\etbbt.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\toolbar\toolbar.c
# End Source File
# End Group
# Begin Group "processbar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\processbar\eprogres.c
# End Source File
# End Group
# Begin Group "trackbar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\trackbar\trackbar.c
# End Source File
# End Group
# Begin Group "updown32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\updown32\updown32.c
# End Source File
# End Group
# Begin Group "commctrllib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\commctrl\commctrl.c
# End Source File
# End Group
# Begin Group "canvas"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\canvas\canvas.c
# End Source File
# End Group
# Begin Group "datesel"

# PROP Default_Filter ""
# End Group
# Begin Group "edittime"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\edittime\edittime.c
# End Source File
# End Group
# Begin Group "sliderctrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\sliderctrl\sliderctrl.c
# End Source File
# End Group
# Begin Group "tabctrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\tabctrl\tabctrl.c
# End Source File
# End Group
# Begin Group "imgbrowser"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\imgbrowser\imgbrowser.c
# End Source File
# End Group
# Begin Group "ipaddress"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\ipaddress\ipaddress.c
# End Source File
# End Group
# Begin Group "dateshow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\commctrl\dateshow\dateshow.c
# End Source File
# End Group
# End Group
# Begin Group "thirdpart"

# PROP Default_Filter ""
# Begin Group "angelus"

# PROP Default_Filter ""
# Begin Group "agl_button"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\thirdpart\Angelus\agl_button\agl_button.c
# End Source File
# End Group
# Begin Group "agl_msgbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\thirdpart\Angelus\agl_msgbox\agl_msgbox.c
# End Source File
# End Group
# Begin Group "aglmain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\thirdpart\angelus\aglmain\aglmain.c
# End Source File
# End Group
# Begin Group "agl_dialog"

# PROP Default_Filter ""
# End Group
# End Group
# End Group
# Begin Group "libjpeg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jccoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jccolor.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jchuff.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcinit.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcomapi.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcparam.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcprepct.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jcsample.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jctrans.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdcolor.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdhuff.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdinput.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdmerge.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdpostct.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdsample.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jdtrans.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jerror.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jfdctint.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jidctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jidctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jidctint.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jidctred.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jmemnobs.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jpegapi.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jquant1.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jquant2.c
# End Source File
# Begin Source File

SOURCE=..\..\library\userlib\libjpeg\jutils.c
# End Source File
# End Group
# End Group
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\application\playback\cano_common.h
# End Source File
# Begin Source File

SOURCE=..\..\application\playback\slidesetting.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\cursor1.cur"
# End Source File
# Begin Source File

SOURCE="..\..\arch\i386-win-kernel\kernel\kingmos.rc"
# End Source File
# End Group
# Begin Group "opengl_lib"

# PROP Default_Filter ""
# Begin Group "debug_lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\thirdpart_prj\Mesa-6.2.1_app\src\glut\glut\Debug\glut.lib"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="..\..\thirdpart_prj\Mesa-6.2.1\src\mesa\drivers\windows\gdi_lib\Debug\OPENGL32.LIB"
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "release_lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\thirdpart_prj\Mesa-6.2.1_app\src\glut\glut\Release\glut.lib"
# End Source File
# Begin Source File

SOURCE="..\..\thirdpart_prj\Mesa-6.2.1\src\mesa\drivers\windows\gdi_lib\Release\OPENGL32.LIB"
# End Source File
# End Group
# End Group
# End Target
# End Project
