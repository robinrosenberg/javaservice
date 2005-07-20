# Microsoft Developer Studio Project File - Name="JavaService" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=JavaService - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "JavaService.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "JavaService.mak" CFG="JavaService - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "JavaService - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "JavaService - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "JavaService - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\j2sdk\include" /I "..\..\j2sdk\include\win32" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 advapi32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "JavaService - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\j2sdk\include" /I "..\..\j2sdk\include\win32" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib /nologo /subsystem:console /incremental:no /debug /machine:I386 /out:"Debug/JavaServiceDebug.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "JavaService - Win32 Release"
# Name "JavaService - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CommandFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\DaemonCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\EventLogger.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\InstallCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\JavaInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\JavaService.cpp
# End Source File
# Begin Source File

SOURCE=.\JavaService.rc
# End Source File
# Begin Source File

SOURCE=.\LicenceCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessGlobals.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\RegistryHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ServiceCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\ServiceHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ServiceLogger.cpp
# End Source File
# Begin Source File

SOURCE=.\ServiceParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\UninstallCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\VersionCommand.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CommandFactory.h
# End Source File
# Begin Source File

SOURCE=.\CommandInterface.h
# End Source File
# Begin Source File

SOURCE=.\DaemonCommand.h
# End Source File
# Begin Source File

SOURCE=.\EventLogger.h
# End Source File
# Begin Source File

SOURCE=.\HelpCommand.h
# End Source File
# Begin Source File

SOURCE=.\InstallCommand.h
# End Source File
# Begin Source File

SOURCE=.\JavaInterface.h
# End Source File
# Begin Source File

SOURCE=.\LicenceCommand.h
# End Source File
# Begin Source File

SOURCE=.\Messages.h
# End Source File
# Begin Source File

SOURCE=.\ProcessGlobals.h
# End Source File
# Begin Source File

SOURCE=.\QueryCommand.h
# End Source File
# Begin Source File

SOURCE=.\RegistryHandler.h
# End Source File
# Begin Source File

SOURCE=.\ServiceCommand.h
# End Source File
# Begin Source File

SOURCE=.\ServiceHandler.h
# End Source File
# Begin Source File

SOURCE=.\ServiceLogger.h
# End Source File
# Begin Source File

SOURCE=.\ServiceParameters.h
# End Source File
# Begin Source File

SOURCE=.\StatusCommand.h
# End Source File
# Begin Source File

SOURCE=.\UninstallCommand.h
# End Source File
# Begin Source File

SOURCE=.\VersionCommand.h
# End Source File
# Begin Source File

SOURCE=.\VersionNo.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\JavaService.rc2
# End Source File
# Begin Source File

SOURCE=.\Messages.mc

!IF  "$(CFG)" == "JavaService - Win32 Release"

# Begin Custom Build - Compiling messages...
InputDir=.
InputPath=.\Messages.mc
InputName=Messages

BuildCmds= \
	mc.exe "$(InputDir)\$(InputName).mc" -r "$(InputDir)" -h "$(InputDir)"

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\MSG00001.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "JavaService - Win32 Debug"

# Begin Custom Build - Compiling messages...
InputDir=.
InputPath=.\Messages.mc
InputName=Messages

BuildCmds= \
	mc.exe "$(InputDir)\$(InputName).mc" -r "$(InputDir)" -h "$(InputDir)"

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\MSG00001.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\UpdateVersion.dsm
# End Source File
# End Target
# End Project
