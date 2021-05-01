# Microsoft Developer Studio Project File - Name="vsthost" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=vsthost - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vsthost23.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsthost23.mak" CFG="vsthost - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsthost - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "vsthost - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "vsthost - Win32 Release Trace" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vsthost - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release23"
# PROP Intermediate_Dir "Release23"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\vstsdk2.3\source\common" /I "..\asiosdk2\common" /I "..\asiosdk2\host" /I "..\asiosdk2\host\pc" /D "NDEBUG" /D "USE_ASIO" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc07 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc07 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "vsthost - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug23"
# PROP Intermediate_Dir "Debug23"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\asiosdk2\host\pc" /I "..\vstsdk2.3\source\common" /I "..\asiosdk2\common" /I "..\asiosdk2\host" /D "_DEBUG" /D "USE_ASIO" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc07 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc07 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "vsthost - Win32 Release Trace"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vsthost___Win32_Release_Trace"
# PROP BASE Intermediate_Dir "vsthost___Win32_Release_Trace"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TRelease23"
# PROP Intermediate_Dir "TRelease23"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\vstsdk2.3\source\common" /I "..\asiosdk2\common" /I "..\asiosdk2\host" /I "..\asiosdk2\host\pc" /D "NDEBUG" /D "USE_ASIO" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\vstsdk2.3\source\common" /I "..\asiosdk2\common" /I "..\asiosdk2\host" /I "..\asiosdk2\host\pc" /D "NDEBUG" /D "USE_ASIO" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_DEBUGFILE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc07 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc07 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /machine:I386

!ENDIF 

# Begin Target

# Name "vsthost - Win32 Release"
# Name "vsthost - Win32 Debug"
# Name "vsthost - Win32 Release Trace"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AsioChannelSelectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AsioHost.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\CVSTHost.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\DSoundDev.cpp
# End Source File
# Begin Source File

SOURCE=.\EffChainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EffectWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\EffEditWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\EffMidiChn.cpp
# End Source File
# Begin Source File

SOURCE=.\EffSecWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\mfcmidi.cpp
# End Source File
# Begin Source File

SOURCE=.\mfcwave.cpp
# End Source File
# Begin Source File

SOURCE=.\mididev.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiKeybDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgNameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SmpVSTHost.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecAsioHost.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecDSound.cpp
# End Source File
# Begin Source File

SOURCE=.\specmidi.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecWave.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\vsthost.cpp
# End Source File
# Begin Source File

SOURCE=.\vsthost.rc
# End Source File
# Begin Source File

SOURCE=.\WaveDev.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkThread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AsioChannelSelectDialog.h
# End Source File
# Begin Source File

SOURCE=.\AsioHost.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\CVSTHost.h
# End Source File
# Begin Source File

SOURCE=.\DSoundDev.h
# End Source File
# Begin Source File

SOURCE=.\EffChainDlg.h
# End Source File
# Begin Source File

SOURCE=.\EffectWnd.h
# End Source File
# Begin Source File

SOURCE=.\EffEditWnd.h
# End Source File
# Begin Source File

SOURCE=.\EffMidiChn.h
# End Source File
# Begin Source File

SOURCE=.\EffSecWnd.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\mfcmidi.h
# End Source File
# Begin Source File

SOURCE=.\mfcwave.h
# End Source File
# Begin Source File

SOURCE=.\mididev.h
# End Source File
# Begin Source File

SOURCE=.\MidiKeybDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProgNameDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ShellSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\SmpVSTHost.h
# End Source File
# Begin Source File

SOURCE=.\SpecAsioHost.h
# End Source File
# Begin Source File

SOURCE=.\SpecDSound.h
# End Source File
# Begin Source File

SOURCE=.\specmidi.h
# End Source File
# Begin Source File

SOURCE=.\SpecWave.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\vsthost.h
# End Source File
# Begin Source File

SOURCE=.\vstsysex.h
# End Source File
# Begin Source File

SOURCE=.\WaveDev.h
# End Source File
# Begin Source File

SOURCE=.\WorkThread.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\backgnd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\effedit.ico
# End Source File
# Begin Source File

SOURCE=.\res\effinfo.ico
# End Source File
# Begin Source File

SOURCE=.\res\effparm.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\kbd.ico
# End Source File
# Begin Source File

SOURCE=.\res\manifest.xml
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\vsthost.ico
# End Source File
# Begin Source File

SOURCE=.\res\vsthost.rc2
# End Source File
# End Group
# Begin Group "ASIO Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\asiosdk2\common\asio.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\asiosdk2\common\asio.h
# End Source File
# Begin Source File

SOURCE=..\asiosdk2\host\asiodrivers.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\asiosdk2\host\asiodrivers.h
# End Source File
# Begin Source File

SOURCE=..\asiosdk2\host\pc\asiolist.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\asiosdk2\host\pc\asiolist.h
# End Source File
# Begin Source File

SOURCE=..\asiosdk2\common\asiosys.h
# End Source File
# Begin Source File

SOURCE=..\asiosdk2\host\ginclude.h
# End Source File
# Begin Source File

SOURCE=..\asiosdk2\common\iasiodrv.h
# End Source File
# End Group
# Begin Group "VSTSDK 2.3 Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\vstsdk2.3\source\common\AEffect.h
# End Source File
# Begin Source File

SOURCE=..\vstsdk2.3\source\common\aeffectx.h
# End Source File
# Begin Source File

SOURCE=..\vstsdk2.3\source\common\AEffEditor.hpp
# End Source File
# Begin Source File

SOURCE=..\vstsdk2.3\source\common\AudioEffect.hpp
# End Source File
# Begin Source File

SOURCE=..\vstsdk2.3\source\common\audioeffectx.h
# End Source File
# Begin Source File

SOURCE=..\vstsdk2.3\source\common\vstfxstore.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\license.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
