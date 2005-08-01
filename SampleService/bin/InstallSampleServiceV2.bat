@echo off

@rem note that if JVM not found, service 'does not report an error' when startup fails, although event logged
set JVMDIR=d:\j2sdk1.4.2_07\jre\bin\server
set SSBINDIR=d:\javaservice\dev\SampleService\bin
set JS1BINDIR=d:\javaservice\dev\javaservice1\bin
set JS2BINDIR=d:\javaservice\dev\javaservice\bin
@rem following line can be edited to switch between JavaService 1.2 and 2.x
set JSBINDIR=%JS2BINDIR%

@echo Installing sample service... Press Control-C to abort
@pause
call installSampleService %JVMDIR% %SSBINDIR% %JSBINDIR% -manual


@echo Starting sample service... Press Control-C to abort
@pause
net start SampleService


@echo Stopping sample service... Press Control-C to abort
@pause
net stop SampleService


@echo Un-installing sample service... Press Control-C to abort
@pause
call uninstallSampleService %SSBINDIR%


@echo End of script
@pause