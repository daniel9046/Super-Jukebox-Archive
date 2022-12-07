# Microsoft Developer Studio Project File - Name="Super Jukebox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Super Jukebox - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Super Jukebox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Super Jukebox.mak" CFG="Super Jukebox - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Super Jukebox - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Super Jukebox - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Super Jukebox - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Ox /Ot /Og /Oi /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib msimg32.lib winmm.lib /nologo /version:3.1 /stack:0x100000,0x10000 /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Super Jukebox - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib msimg32.lib winmm.lib /nologo /version:3.0 /stack:0x100000,0x10000 /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Super Jukebox - Win32 Release"
# Name "Super Jukebox - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ActiveWallpaper.cpp
# End Source File
# Begin Source File

SOURCE=.\ActiveWallpaperDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AddFilesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Application.cpp
# End Source File
# Begin Source File

SOURCE=.\BetaBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigurationDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlPanelDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Extractor.cpp
# End Source File
# Begin Source File

SOURCE=.\FFTlib.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\FileList.cpp
# End Source File
# Begin Source File

SOURCE=.\FindDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\GotoDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\LabelEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\List.cpp
# End Source File
# Begin Source File

SOURCE=.\ListEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Menu.cpp
# End Source File
# Begin Source File

SOURCE=.\MiscDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayList.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayListDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PlaylistFilterDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgramDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ReBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Registry.cpp
# End Source File
# Begin Source File

SOURCE=.\Skin.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SourceBar.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=".\Super Jukebox.rc"
# End Source File
# Begin Source File

SOURCE=.\ToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolbarCustomizeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualizationDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\VisWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\WarningDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Wnd.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDialog.h
# End Source File
# Begin Source File

SOURCE=.\ActiveWallpaper.h
# End Source File
# Begin Source File

SOURCE=.\ActiveWallpaperDialog.h
# End Source File
# Begin Source File

SOURCE=.\AddFilesDialog.h
# End Source File
# Begin Source File

SOURCE=.\Application.h
# End Source File
# Begin Source File

SOURCE=.\BetaBox.h
# End Source File
# Begin Source File

SOURCE=.\ConfigurationDialog.h
# End Source File
# Begin Source File

SOURCE=.\ControlPanelDialog.h
# End Source File
# Begin Source File

SOURCE=.\Extractor.h
# End Source File
# Begin Source File

SOURCE=.\FileList.h
# End Source File
# Begin Source File

SOURCE=.\FindDialog.h
# End Source File
# Begin Source File

SOURCE=.\GotoDialog.h
# End Source File
# Begin Source File

SOURCE=.\LabelEdit.h
# End Source File
# Begin Source File

SOURCE=.\List.h
# End Source File
# Begin Source File

SOURCE=.\ListEntry.h
# End Source File
# Begin Source File

SOURCE=.\Main.h
# End Source File
# Begin Source File

SOURCE=.\MainWindow.h
# End Source File
# Begin Source File

SOURCE=.\Menu.h
# End Source File
# Begin Source File

SOURCE=.\MiscDialog.h
# End Source File
# Begin Source File

SOURCE=.\Player.h
# End Source File
# Begin Source File

SOURCE=.\PlayerDialog.h
# End Source File
# Begin Source File

SOURCE=.\PlayList.h
# End Source File
# Begin Source File

SOURCE=.\PlayListDialog.h
# End Source File
# Begin Source File

SOURCE=.\PlaylistFilterDialog.h
# End Source File
# Begin Source File

SOURCE=.\ProgramDialog.h
# End Source File
# Begin Source File

SOURCE=.\ReBar.h
# End Source File
# Begin Source File

SOURCE=.\Registry.h
# End Source File
# Begin Source File

SOURCE=.\resource.hm
# End Source File
# Begin Source File

SOURCE=.\Skin.h
# End Source File
# Begin Source File

SOURCE=.\SkinDialog.h
# End Source File
# Begin Source File

SOURCE=.\SourceBar.h
# End Source File
# Begin Source File

SOURCE=.\StatusBar.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ToolBar.h
# End Source File
# Begin Source File

SOURCE=.\ToolbarCustomizeDialog.h
# End Source File
# Begin Source File

SOURCE=.\ViewDialog.h
# End Source File
# Begin Source File

SOURCE=.\VisualizationDialog.h
# End Source File
# Begin Source File

SOURCE=.\VisWindow.h
# End Source File
# Begin Source File

SOURCE=.\WarningDialog.h
# End Source File
# Begin Source File

SOURCE=.\Wnd.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ColorIcons.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DownUpArrows.bmp
# End Source File
# Begin Source File

SOURCE=.\res\GrayIcons.bmp
# End Source File
# Begin Source File

SOURCE=.\res\oneup_color.bmp
# End Source File
# Begin Source File

SOURCE=.\res\oneup_gray.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SNCart.ico
# End Source File
# Begin Source File

SOURCE=".\res\Super Jukebox.ico"
# End Source File
# End Group
# Begin Group "Import Libraries"

# PROP Default_Filter ".lib"
# Begin Source File

SOURCE=.\zlib.lib
# End Source File
# Begin Source File

SOURCE=.\unrar.lib
# End Source File
# Begin Source File

SOURCE=.\Uematsu.lib
# End Source File
# End Group
# End Target
# End Project
