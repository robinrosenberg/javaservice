@echo off

if "%1" == "" goto err
if "%2" == "" goto err
if "%3" == "" goto err

goto ok

:err

echo --------
echo -
echo - Usage:   %0 jvm_dir sample_dir javaservice_dir [-auto / -manual]
echo -
echo - Example: %0 c:\j2sdk1.4.2_06\jre\bin\server d:\javaservice\dev\SampleService d:\javaservice\dev\JavaService\bin -manual
echo -
echo - Note: Spaces are not permitted in these path names.
echo -
echo --------
goto eof

:ok

@echo .
@echo .
@echo .

copy %3\JavaService.exe %2\SampleService.exe > nul

%2\SampleService.exe -install SampleService %1\jvm.dll -Djava.class.path=%2\;%1\lib\tools.jar -Xms16M -Xmx64M -start org.objectweb.javaservice.test.SampleService -params start -stop org.objectweb.javaservice.test.SampleService -method serviceStop -out %2\stdout.log -err %2\stderr.log -current %2 %4

@echo .
@echo .
@echo .

:eof

@pause
