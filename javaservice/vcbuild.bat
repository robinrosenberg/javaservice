@rem vcbuild batch file to perform Visual Studio build steps
@rem first, need to set up command-line values to run visual studio commands
@rem call vcvars32
@rem use the exported project makefile for both JavaService targets
pushd src
nmake /nologo /s /f javaservice.mak CFG="JavaService - Win32 Release"
nmake /nologo /s /f javaservice.mak CFG="JavaService - Win32 Debug"
popd
@pause
