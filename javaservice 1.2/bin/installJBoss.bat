@echo off

if "%1" == "" goto err
if "%2" == "" goto err
if "%3" == "" goto err
if "%4" == "" goto err

goto ok

:err

echo --------
echo Usage:
echo   %0 jdk_home jboss_home (classic/hotspot/server) other_service [-auto / -manual]
echo NOTE:    You MAY NOT use spaces in the path names (tell me if you can fix this).
echo          JDK 1.3 does not come with hotpot server by default, you must
echo          install this separately if you wish to use it.
echo Example: %0 c:\jdk1.3.1_04 c:\jboss hotspot mysql -auto
echo --------
goto eof

:ok

@echo .
@echo .
@echo .

copy JavaService.exe %2\bin\JBossService.exe > nul

%2\bin\JBossService.exe -install JBoss %1\jre\bin\%3\jvm.dll -Djava.class.path=%1\lib\tools.jar;%2\bin\run.jar -start org.jboss.Main -stop org.jboss.Main -method systemExit -out %2\bin\out.log -err %2\bin\err.log -current %2\bin -depends %4 %5 -overwrite

goto eof

:eof

@echo .
@echo .
@echo .
