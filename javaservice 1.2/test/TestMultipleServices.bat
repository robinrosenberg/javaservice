@echo Perform test of multiple service dependencies

setlocal

@rem note, following directory path will cause problems if it includes spaces
set TSTDIR="%CD%"
set JVMDLL=d:\j2sdk1.4.2_07\jre\bin\server\jvm.dll
set SSJAR=..\..\SampleService\bin\SampleService.jar
set JSEXE=..\bin\JavaService.exe

@rem Copy/rename exe so Task Manager displays useful filename(s)
copy %JSEXE% TestServiceAAA.exe > nul
copy %JSEXE% TestServiceBBB.exe > nul
copy %JSEXE% TestServiceCCC.exe > nul

@pause

@rem Invoke the executables to install the two 'parent' services
TestServiceAAA.exe -install TestServiceAAA %JVMDLL% -Djava.class.path=%SSJAR% -start org.objectweb.javaservice.test.SampleService -stop org.objectweb.javaservice.test.SampleService -method serviceStop -out %TSTDIR%\aaaout.log -err %TSTDIR%\aaaerr.log -current %TSTDIR% -manual
TestServiceBBB.exe -install TestServiceBBB %JVMDLL% -Djava.class.path=%SSJAR% -start org.objectweb.javaservice.test.SampleService -stop org.objectweb.javaservice.test.SampleService -method serviceStop -out %TSTDIR%\bbbout.log -err %TSTDIR%\bbberr.log -current %TSTDIR% -manual

@pause

@rem Check that one of these services can be stopped/started normally
net start TestServiceAAA
@pause
net stop TestServiceAAA
@pause


@rem Install the 'child' service, dependant on both parents.
TestServiceCCC.exe -install TestServiceCCC %JVMDLL% -Djava.class.path=%SSJAR% -start org.objectweb.javaservice.test.SampleService -stop org.objectweb.javaservice.test.SampleService -method serviceStop -out %TSTDIR%\cccout.log -err %TSTDIR%\cccerr.log -current %TSTDIR% -depends TestServiceAAA,TestServiceBBB -manual

@pause

@rem Attempt to start child process, should run up others too
net start TestServiceCCC

@echo Check log file output - all three instances should now be running
@pause

@rem Stop the services - all three should be running at this time
net stop TestServiceCCC
net stop TestServiceBBB
net stop TestServiceAAA

@pause

@rem Uninstall the test services
TestServiceAAA.exe -uninstall TestServiceAAA
TestServiceBBB.exe -uninstall TestServiceBBB
TestServiceCCC.exe -uninstall TestServiceCCC


@pause

@rem Remove the temporary copies of executables as final cleanup
del TestServiceAAA.exe
del TestServiceBBB.exe
del TestServiceCCC.exe



@pause
