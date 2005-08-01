@rem echo off

if "%1" == "" goto err
if "%2" == "" goto err
if "%3" == "" goto err

goto ok

:err

echo --------
echo -
echo - Usage:   %0 jvm_dir sample_dir javaservice_dir [-auto / -manual]
echo -
echo - Example: %0 c:\j2sdk1.4.2\jre\bin\server C:\javaservice\dev\SampleService C:\javaservice\dev\JavaService\bin -manual
echo -
echo - Note: Spaces are not permitted in these path names.
echo -
echo --------
goto eof

:ok

@echo .

@rem Copy/rename file so Task Manager displays useful filename
copy %3\JavaService.exe %2\SampleService.exe > nul

@echo . Using following version of JavaService executable:
%2\SampleService.exe -version

@echo .

@rem Invoke the executable to install the service
%2\SampleService.exe -install SampleService %1\jvm.dll -Djava.class.path=%2\SampleService.jar -Xms16M -Xmx32M -start org.objectweb.javaservice.test.SampleService -params start memory -stop org.objectweb.javaservice.test.SampleService -method serviceStop -out %2\stdout.log -err %2\stderr.log -current %2 %4 -append -startup 3

@echo .
@echo .
@echo .

:eof

@pause
