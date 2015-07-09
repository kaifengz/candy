# Microsoft Developer Studio Project File - Name="RJ" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RJ - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RJ.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RJ.mak" CFG="RJ - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RJ - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RJ - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RJ - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Z<none> /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"Release/RJ2.0.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "RJ - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "RJ - Win32 Release"
# Name "RJ - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AI.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\RJ.cpp
# End Source File
# Begin Source File

SOURCE=.\RJ.rc
# End Source File
# Begin Source File

SOURCE=.\ScoreDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\defines.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RJ.h
# End Source File
# Begin Source File

SOURCE=.\ScoreDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\card01.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card02.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card03.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card04.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card05.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card06.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card07.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card08.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card09.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card11.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card12.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card13.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card14.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card15.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card17.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card18.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card19.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card20.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card21.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card22.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card23.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card24.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card25.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card26.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card27.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card28.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card29.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card30.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card31.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card32.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card33.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card34.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card35.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card36.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card37.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card38.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card39.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card40.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card41.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card42.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card43.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card44.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card45.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card46.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card47.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card48.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card49.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card50.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card51.bmp
# End Source File
# Begin Source File

SOURCE=.\res\card52.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CardBack.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CardShape.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CardSharp.bmp
# End Source File
# Begin Source File

SOURCE=.\res\felt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\RJ.ico
# End Source File
# Begin Source File

SOURCE=.\res\RJ.rc2
# End Source File
# Begin Source File

SOURCE=.\res\token.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TokenShape.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TokenSharp.bmp
# End Source File
# End Group
# End Target
# End Project
