@echo off

if "%1" == "" goto err
if "%2" == "" goto err

goto ok

:err

echo --------
echo Usage:
echo   %0 jdk_home (classic/hotspot/client/server)
echo --------
goto eof

:ok
set jsdev=c:\javaservice\dev

JavaService.exe -install OutputTest %1\jre\bin\%2\jvm.dll -Djava.class.path=%jsdev%\JavaServiceTest\bin -start OutputTest -stop OutputTest -method systemExit -out %jsdev%\JavaServiceTest\out.log -err %jsdev%\JavaServiceTest\err.log -current %jsdev%\JavaServiceTest -manual

:eof

