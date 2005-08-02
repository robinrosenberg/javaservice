@echo off

@echo .
@echo . TestSampleService.bat - test JavaService executable using SampleService.jar
@echo .


setlocal
@rem note that if JVM not found, service 'does not report an error' when startup fails, although event logged
if "%JAVA_HOME%" == "" set JAVA_HOME=d:\j2sdk1.4.2_07\jre
set JVMDIR=%JAVA_HOME%\jre\bin\server
set JSBINDIR=%CD%
set JSEXE=%JSBINDIR%\JavaService.exe
set SSBINDIR=%JSBINDIR%


@echo . Using following version of JavaService executable:
@echo .
%JSEXE% -version
@echo .


@echo Installing sample service... Press Control-C to abort
@pause
@echo .
%JSEXE% -install SampleService %JVMDIR%\jvm.dll -Djava.class.path=%SSBINDIR%\SampleService.jar -Xms16M -Xmx32M -start org.objectweb.javaservice.test.SampleService -params start memory -stop org.objectweb.javaservice.test.SampleService -method serviceStop -out %JSBINDIR%\stdout.log -err %JSBINDIR%\stderr.log -current %JSBINDIR% -manual
@echo .


@echo Starting sample service... Press Control-C to abort
@pause
@echo .
net start SampleService
@echo .


@echo Stopping sample service... Press Control-C to abort
@pause
@echo .
net stop SampleService
@echo .


@echo Un-installing sample service... Press Control-C to abort
@pause
@echo .
%JSEXE% -uninstall SampleService
@echo .


@echo End of script
@pause