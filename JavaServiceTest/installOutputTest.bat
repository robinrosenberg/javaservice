@echo off

if "%1" == "" goto err
if "%2" == "" goto err

goto ok

:err

echo --------
echo Usage:
echo   %0 jdk_home (classic/hotspot/server)
echo --------
goto eof

:ok

JavaService.exe -install OutputTest %1\jre\bin\%2\jvm.dll -Djava.class.path=d:\javaservice\dev\JavaServiceTest\bin -start OutputTest -stop OutputTest -method systemExit -out d:\javaservice\dev\JavaServiceTest\out.log -err d:\Multiplan\dev\JavaServiceTest\err.log -current d:\javaservice\dev\JavaServiceTest -manual

:eof

