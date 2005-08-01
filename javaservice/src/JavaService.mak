# Microsoft Developer Studio Generated NMAKE File, Based on JavaService.dsp
!IF "$(CFG)" == ""
CFG=JavaService - Win32 Debug
!MESSAGE No configuration specified. Defaulting to JavaService - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "JavaService - Win32 Release" && "$(CFG)" != "JavaService - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "JavaService - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : ".\MSG00001.bin" ".\Messages.rc" ".\Messages.h" "$(OUTDIR)\JavaService.exe" "$(OUTDIR)\JavaService.bsc"


CLEAN :
	-@erase "$(INTDIR)\CommandFactory.obj"
	-@erase "$(INTDIR)\CommandFactory.sbr"
	-@erase "$(INTDIR)\DaemonCommand.obj"
	-@erase "$(INTDIR)\DaemonCommand.sbr"
	-@erase "$(INTDIR)\EventLogger.obj"
	-@erase "$(INTDIR)\EventLogger.sbr"
	-@erase "$(INTDIR)\HelpCommand.obj"
	-@erase "$(INTDIR)\HelpCommand.sbr"
	-@erase "$(INTDIR)\InstallCommand.obj"
	-@erase "$(INTDIR)\InstallCommand.sbr"
	-@erase "$(INTDIR)\JavaInterface.obj"
	-@erase "$(INTDIR)\JavaInterface.sbr"
	-@erase "$(INTDIR)\JavaService.obj"
	-@erase "$(INTDIR)\JavaService.res"
	-@erase "$(INTDIR)\JavaService.sbr"
	-@erase "$(INTDIR)\LicenceCommand.obj"
	-@erase "$(INTDIR)\LicenceCommand.sbr"
	-@erase "$(INTDIR)\ProcessGlobals.obj"
	-@erase "$(INTDIR)\ProcessGlobals.sbr"
	-@erase "$(INTDIR)\QueryCommand.obj"
	-@erase "$(INTDIR)\QueryCommand.sbr"
	-@erase "$(INTDIR)\RegistryHandler.obj"
	-@erase "$(INTDIR)\RegistryHandler.sbr"
	-@erase "$(INTDIR)\ServiceCommand.obj"
	-@erase "$(INTDIR)\ServiceCommand.sbr"
	-@erase "$(INTDIR)\ServiceHandler.obj"
	-@erase "$(INTDIR)\ServiceHandler.sbr"
	-@erase "$(INTDIR)\ServiceLogger.obj"
	-@erase "$(INTDIR)\ServiceLogger.sbr"
	-@erase "$(INTDIR)\ServiceParameters.obj"
	-@erase "$(INTDIR)\ServiceParameters.sbr"
	-@erase "$(INTDIR)\StatusCommand.obj"
	-@erase "$(INTDIR)\StatusCommand.sbr"
	-@erase "$(INTDIR)\UninstallCommand.obj"
	-@erase "$(INTDIR)\UninstallCommand.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\VersionCommand.obj"
	-@erase "$(INTDIR)\VersionCommand.sbr"
	-@erase "$(OUTDIR)\JavaService.bsc"
	-@erase "$(OUTDIR)\JavaService.exe"
	-@erase ".\Messages.h"
	-@erase ".\Messages.rc"
	-@erase ".\MSG00001.bin"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\j2sdk\include" /I "..\..\j2sdk\include\win32" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\JavaService.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\JavaService.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\JavaService.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CommandFactory.sbr" \
	"$(INTDIR)\DaemonCommand.sbr" \
	"$(INTDIR)\EventLogger.sbr" \
	"$(INTDIR)\HelpCommand.sbr" \
	"$(INTDIR)\InstallCommand.sbr" \
	"$(INTDIR)\JavaInterface.sbr" \
	"$(INTDIR)\JavaService.sbr" \
	"$(INTDIR)\LicenceCommand.sbr" \
	"$(INTDIR)\ProcessGlobals.sbr" \
	"$(INTDIR)\QueryCommand.sbr" \
	"$(INTDIR)\RegistryHandler.sbr" \
	"$(INTDIR)\ServiceCommand.sbr" \
	"$(INTDIR)\ServiceHandler.sbr" \
	"$(INTDIR)\ServiceLogger.sbr" \
	"$(INTDIR)\ServiceParameters.sbr" \
	"$(INTDIR)\StatusCommand.sbr" \
	"$(INTDIR)\UninstallCommand.sbr" \
	"$(INTDIR)\VersionCommand.sbr"

"$(OUTDIR)\JavaService.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=advapi32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\JavaService.pdb" /machine:I386 /out:"$(OUTDIR)\JavaService.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CommandFactory.obj" \
	"$(INTDIR)\DaemonCommand.obj" \
	"$(INTDIR)\EventLogger.obj" \
	"$(INTDIR)\HelpCommand.obj" \
	"$(INTDIR)\InstallCommand.obj" \
	"$(INTDIR)\JavaInterface.obj" \
	"$(INTDIR)\JavaService.obj" \
	"$(INTDIR)\LicenceCommand.obj" \
	"$(INTDIR)\ProcessGlobals.obj" \
	"$(INTDIR)\QueryCommand.obj" \
	"$(INTDIR)\RegistryHandler.obj" \
	"$(INTDIR)\ServiceCommand.obj" \
	"$(INTDIR)\ServiceHandler.obj" \
	"$(INTDIR)\ServiceLogger.obj" \
	"$(INTDIR)\ServiceParameters.obj" \
	"$(INTDIR)\StatusCommand.obj" \
	"$(INTDIR)\UninstallCommand.obj" \
	"$(INTDIR)\VersionCommand.obj" \
	"$(INTDIR)\JavaService.res"

"$(OUTDIR)\JavaService.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "JavaService - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\JavaServiceDebug.exe" "$(OUTDIR)\JavaService.bsc"


CLEAN :
	-@erase "$(INTDIR)\CommandFactory.obj"
	-@erase "$(INTDIR)\CommandFactory.sbr"
	-@erase "$(INTDIR)\DaemonCommand.obj"
	-@erase "$(INTDIR)\DaemonCommand.sbr"
	-@erase "$(INTDIR)\EventLogger.obj"
	-@erase "$(INTDIR)\EventLogger.sbr"
	-@erase "$(INTDIR)\HelpCommand.obj"
	-@erase "$(INTDIR)\HelpCommand.sbr"
	-@erase "$(INTDIR)\InstallCommand.obj"
	-@erase "$(INTDIR)\InstallCommand.sbr"
	-@erase "$(INTDIR)\JavaInterface.obj"
	-@erase "$(INTDIR)\JavaInterface.sbr"
	-@erase "$(INTDIR)\JavaService.obj"
	-@erase "$(INTDIR)\JavaService.res"
	-@erase "$(INTDIR)\JavaService.sbr"
	-@erase "$(INTDIR)\LicenceCommand.obj"
	-@erase "$(INTDIR)\LicenceCommand.sbr"
	-@erase "$(INTDIR)\ProcessGlobals.obj"
	-@erase "$(INTDIR)\ProcessGlobals.sbr"
	-@erase "$(INTDIR)\QueryCommand.obj"
	-@erase "$(INTDIR)\QueryCommand.sbr"
	-@erase "$(INTDIR)\RegistryHandler.obj"
	-@erase "$(INTDIR)\RegistryHandler.sbr"
	-@erase "$(INTDIR)\ServiceCommand.obj"
	-@erase "$(INTDIR)\ServiceCommand.sbr"
	-@erase "$(INTDIR)\ServiceHandler.obj"
	-@erase "$(INTDIR)\ServiceHandler.sbr"
	-@erase "$(INTDIR)\ServiceLogger.obj"
	-@erase "$(INTDIR)\ServiceLogger.sbr"
	-@erase "$(INTDIR)\ServiceParameters.obj"
	-@erase "$(INTDIR)\ServiceParameters.sbr"
	-@erase "$(INTDIR)\StatusCommand.obj"
	-@erase "$(INTDIR)\StatusCommand.sbr"
	-@erase "$(INTDIR)\UninstallCommand.obj"
	-@erase "$(INTDIR)\UninstallCommand.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\VersionCommand.obj"
	-@erase "$(INTDIR)\VersionCommand.sbr"
	-@erase "$(OUTDIR)\JavaService.bsc"
	-@erase "$(OUTDIR)\JavaServiceDebug.exe"
	-@erase "$(OUTDIR)\JavaServiceDebug.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "..\..\j2sdk\include" /I "..\..\j2sdk\include\win32" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\JavaService.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\JavaService.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\JavaService.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CommandFactory.sbr" \
	"$(INTDIR)\DaemonCommand.sbr" \
	"$(INTDIR)\EventLogger.sbr" \
	"$(INTDIR)\HelpCommand.sbr" \
	"$(INTDIR)\InstallCommand.sbr" \
	"$(INTDIR)\JavaInterface.sbr" \
	"$(INTDIR)\JavaService.sbr" \
	"$(INTDIR)\LicenceCommand.sbr" \
	"$(INTDIR)\ProcessGlobals.sbr" \
	"$(INTDIR)\QueryCommand.sbr" \
	"$(INTDIR)\RegistryHandler.sbr" \
	"$(INTDIR)\ServiceCommand.sbr" \
	"$(INTDIR)\ServiceHandler.sbr" \
	"$(INTDIR)\ServiceLogger.sbr" \
	"$(INTDIR)\ServiceParameters.sbr" \
	"$(INTDIR)\StatusCommand.sbr" \
	"$(INTDIR)\UninstallCommand.sbr" \
	"$(INTDIR)\VersionCommand.sbr"

"$(OUTDIR)\JavaService.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=advapi32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\JavaServiceDebug.pdb" /debug /machine:I386 /out:"$(OUTDIR)\JavaServiceDebug.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\CommandFactory.obj" \
	"$(INTDIR)\DaemonCommand.obj" \
	"$(INTDIR)\EventLogger.obj" \
	"$(INTDIR)\HelpCommand.obj" \
	"$(INTDIR)\InstallCommand.obj" \
	"$(INTDIR)\JavaInterface.obj" \
	"$(INTDIR)\JavaService.obj" \
	"$(INTDIR)\LicenceCommand.obj" \
	"$(INTDIR)\ProcessGlobals.obj" \
	"$(INTDIR)\QueryCommand.obj" \
	"$(INTDIR)\RegistryHandler.obj" \
	"$(INTDIR)\ServiceCommand.obj" \
	"$(INTDIR)\ServiceHandler.obj" \
	"$(INTDIR)\ServiceLogger.obj" \
	"$(INTDIR)\ServiceParameters.obj" \
	"$(INTDIR)\StatusCommand.obj" \
	"$(INTDIR)\UninstallCommand.obj" \
	"$(INTDIR)\VersionCommand.obj" \
	"$(INTDIR)\JavaService.res"

"$(OUTDIR)\JavaServiceDebug.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("JavaService.dep")
!INCLUDE "JavaService.dep"
!ELSE 
!MESSAGE Warning: cannot find "JavaService.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "JavaService - Win32 Release" || "$(CFG)" == "JavaService - Win32 Debug"
SOURCE=.\CommandFactory.cpp

"$(INTDIR)\CommandFactory.obj"	"$(INTDIR)\CommandFactory.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DaemonCommand.cpp

"$(INTDIR)\DaemonCommand.obj"	"$(INTDIR)\DaemonCommand.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\EventLogger.cpp

"$(INTDIR)\EventLogger.obj"	"$(INTDIR)\EventLogger.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HelpCommand.cpp

"$(INTDIR)\HelpCommand.obj"	"$(INTDIR)\HelpCommand.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\InstallCommand.cpp

"$(INTDIR)\InstallCommand.obj"	"$(INTDIR)\InstallCommand.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\JavaInterface.cpp

"$(INTDIR)\JavaInterface.obj"	"$(INTDIR)\JavaInterface.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\JavaService.cpp

"$(INTDIR)\JavaService.obj"	"$(INTDIR)\JavaService.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\JavaService.rc

"$(INTDIR)\JavaService.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\LicenceCommand.cpp

"$(INTDIR)\LicenceCommand.obj"	"$(INTDIR)\LicenceCommand.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ProcessGlobals.cpp

"$(INTDIR)\ProcessGlobals.obj"	"$(INTDIR)\ProcessGlobals.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\QueryCommand.cpp

"$(INTDIR)\QueryCommand.obj"	"$(INTDIR)\QueryCommand.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\RegistryHandler.cpp

"$(INTDIR)\RegistryHandler.obj"	"$(INTDIR)\RegistryHandler.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ServiceCommand.cpp

"$(INTDIR)\ServiceCommand.obj"	"$(INTDIR)\ServiceCommand.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ServiceHandler.cpp

"$(INTDIR)\ServiceHandler.obj"	"$(INTDIR)\ServiceHandler.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ServiceLogger.cpp

"$(INTDIR)\ServiceLogger.obj"	"$(INTDIR)\ServiceLogger.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ServiceParameters.cpp

"$(INTDIR)\ServiceParameters.obj"	"$(INTDIR)\ServiceParameters.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StatusCommand.cpp

"$(INTDIR)\StatusCommand.obj"	"$(INTDIR)\StatusCommand.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\UninstallCommand.cpp

"$(INTDIR)\UninstallCommand.obj"	"$(INTDIR)\UninstallCommand.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\VersionCommand.cpp

"$(INTDIR)\VersionCommand.obj"	"$(INTDIR)\VersionCommand.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Messages.mc

!IF  "$(CFG)" == "JavaService - Win32 Release"

InputDir=.
InputPath=.\Messages.mc
InputName=Messages

".\Messages.h"	".\MSG00001.bin"	".\Messages.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	mc.exe "$(InputDir)\$(InputName).mc" -r "$(InputDir)" -h "$(InputDir)"
<< 
	

!ELSEIF  "$(CFG)" == "JavaService - Win32 Debug"

InputDir=.
InputPath=.\Messages.mc
InputName=Messages

".\Messages.h"	".\MSG00001.bin"	".\Messages.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	mc.exe "$(InputDir)\$(InputName).mc" -r "$(InputDir)" -h "$(InputDir)"
<< 
	

!ENDIF 


!ENDIF 

