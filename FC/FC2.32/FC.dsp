# Microsoft Developer Studio Project File - Name="FC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FC.mak" CFG="FC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FC - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FC - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FC - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "FC - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FAs /FR /Yu"stdafx.h" /FD /GZ /c
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

# Name "FC - Win32 Release"
# Name "FC - Win32 Debug"
# Begin Group "Main Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\basic.cpp
# End Source File
# Begin Source File

SOURCE=".\FC-Compile.cpp"
# End Source File
# Begin Source File

SOURCE=".\FC-GenerateFiles.cpp"
# End Source File
# Begin Source File

SOURCE=".\FC-Interpret.cpp"
# End Source File
# Begin Source File

SOURCE=".\FC-MenuMessage.cpp"
# End Source File
# Begin Source File

SOURCE=".\FC-Morphology.cpp"
# End Source File
# Begin Source File

SOURCE=".\FC-Options.cpp"
# End Source File
# Begin Source File

SOURCE=".\FC-Translate.cpp"
# End Source File
# Begin Source File

SOURCE=.\FCErrorReportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FCInputDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FCInterpretDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FCOptionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FCTranslateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FCView.cpp
# End Source File
# End Group
# Begin Group "Main Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\basic.h
# End Source File
# Begin Source File

SOURCE=".\FC-define.h"
# End Source File
# Begin Source File

SOURCE=.\FCErrorReportDlg.h
# End Source File
# Begin Source File

SOURCE=.\FCInputDlg.h
# End Source File
# Begin Source File

SOURCE=.\FCInterpretDlg.h
# End Source File
# Begin Source File

SOURCE=.\FCOptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\FCTranslateDlg.h
# End Source File
# Begin Source File

SOURCE=.\FCView.h
# End Source File
# End Group
# Begin Group "Text Files"

# PROP Default_Filter "txt"
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\syntax.txt
# End Source File
# End Group
# Begin Group "Other Files"

# PROP Default_Filter ""
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\FC.cpp
# End Source File
# Begin Source File

SOURCE=.\FC.rc
# End Source File
# Begin Source File

SOURCE=.\FCDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\common.h
# End Source File
# Begin Source File

SOURCE=.\FC.h
# End Source File
# Begin Source File

SOURCE=.\FCDoc.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\FC.ico
# End Source File
# Begin Source File

SOURCE=.\res\FC.rc2
# End Source File
# Begin Source File

SOURCE=.\res\FCTYPE.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "Extern Resource Files"

# PROP Default_Filter "exe;txt;fc"
# Begin Source File

SOURCE=..\FCR\Release\FCR.exe
# End Source File
# Begin Source File

SOURCE=..\Source\Fibonacci.fc
# End Source File
# Begin Source File

SOURCE=.\res\global.txt
# End Source File
# Begin Source File

SOURCE=.\res\grammar.txt
# End Source File
# Begin Source File

SOURCE=..\source\Hanio.fc
# End Source File
# Begin Source File

SOURCE=..\source\HelloWorld.fc
# End Source File
# Begin Source File

SOURCE=..\source\Permutation.fc
# End Source File
# Begin Source File

SOURCE=..\source\PI.fc
# End Source File
# Begin Source File

SOURCE=..\source\Prime.fc
# End Source File
# End Group
# Begin Source File

SOURCE="..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=.\FC.reg
# End Source File
# End Group
# End Target
# End Project
