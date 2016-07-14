# Microsoft Developer Studio Project File - Name="Freq" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Freq - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Freq.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Freq.mak" CFG="Freq - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Freq - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Freq - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Freq"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Freq - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /subsystem:windows /profile /machine:I386

!ELSEIF  "$(CFG)" == "Freq - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /subsystem:windows /profile /map /debug /machine:I386

!ENDIF 

# Begin Target

# Name "Freq - Win32 Release"
# Name "Freq - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\freq.rc
# End Source File
# Begin Source File

SOURCE=.\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\logo.ico
# End Source File
# Begin Source File

SOURCE=.\record.ico
# End Source File
# Begin Source File

SOURCE=.\rs.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\active.cpp
# End Source File
# Begin Source File

SOURCE=.\active.h
# End Source File
# Begin Source File

SOURCE=.\Canvas.h
# End Source File
# Begin Source File

SOURCE=.\cassette.cpp
# End Source File
# Begin Source File

SOURCE=.\cassette.h
# End Source File
# Begin Source File

SOURCE=.\Complex.h
# End Source File
# Begin Source File

SOURCE=.\Control.cpp
# End Source File
# Begin Source File

SOURCE=.\control.h
# End Source File
# Begin Source File

SOURCE=.\controls.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls.h
# End Source File
# Begin Source File

SOURCE=.\display.cpp
# End Source File
# Begin Source File

SOURCE=.\Display.h
# End Source File
# Begin Source File

SOURCE=.\Fft.cpp
# End Source File
# Begin Source File

SOURCE=.\Fft.h
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\metactrl.h
# End Source File
# Begin Source File

SOURCE=.\params.h
# End Source File
# Begin Source File

SOURCE=.\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\Player.h
# End Source File
# Begin Source File

SOURCE=.\Recorder.cpp
# End Source File
# Begin Source File

SOURCE=.\Recorder.h
# End Source File
# Begin Source File

SOURCE=.\SmartPtrs.cpp
# End Source File
# Begin Source File

SOURCE=.\smartptrs.h
# End Source File
# Begin Source File

SOURCE=.\TapeUser.cpp
# End Source File
# Begin Source File

SOURCE=.\TapeUser.h
# End Source File
# Begin Source File

SOURCE=.\thread.h
# End Source File
# Begin Source File

SOURCE=.\views.cpp
# End Source File
# Begin Source File

SOURCE=.\Views.h
# End Source File
# Begin Source File

SOURCE=.\wassert.h
# End Source File
# Begin Source File

SOURCE=.\WaveDev.h
# End Source File
# Begin Source File

SOURCE=.\winex.h
# End Source File
# Begin Source File

SOURCE=.\xptr.h
# End Source File
# End Target
# End Project
