@echo off

if "%1" == "" goto err
if "%2" == "" goto err
if "%3" == "" goto err
if "%4" == "" goto err
if "%5" == "" goto err
if "%6" == "" goto err

goto ok

:err

echo --------
echo Usage:   %0 jdk_home orion_home (classic/hotspot/server) admin_username admin_password other_service
echo NOTE:    You MAY NOT use spaces in the path names. If you know how
echo          to fix this, please tell me.
echo          JDK 1.3 does not come with hotpot server by default, you must
echo          install this seperately if you wish to use it.
echo Example: %0 c:\jdk1.3.1_01 c:\orion hotspot orionadmin orionnimda mysql
echo --------
goto eof

:ok

@echo .
@echo .
@echo .

copy JavaService.exe %2\OrionService.exe > nul

%2\OrionService.exe -install Orion %1\jre\bin\%3\jvm.dll -Djava.class.path=%2\orion.jar;%1\lib\tools.jar -start com.evermind.server.ApplicationServer -stop com.evermind.client.orion.OrionConsoleAdmin -params "ormi://localhost %4 %5 -shutdown" -out %2\log\stdout.log -err %2\log\stderr.log -current %2 -depends %6

:eof

@echo .
@echo .
@echo .
