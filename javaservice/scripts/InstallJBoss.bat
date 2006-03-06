@echo off

rem * JavaService installation script for JBoss Application Server
rem *
rem * JavaService - Windows NT Service Daemon for Java applications
rem * Copyright (C) 2006 Multiplan Consultants Ltd. LGPL Licensing applies
rem * Information about the JavaService software is available at the ObjectWeb
rem * web site. Refer to http://javaservice.objectweb.org for more details.

SETLOCAL

rem verify that the JavaService exe file is available
if not exist "JavaService.exe" goto no_jsexe


rem check that Java is installed and environment variable set up
if "%JAVA_HOME%" == "" goto no_java
if not exist "%JAVA_HOME%\jre" goto no_java

rem check for any of server, hotspot or client Java run-times
SET jvmdll=%JAVA_HOME%\jre\bin\server\jvm.dll
if not exist "%jvmdll%" SET jvmdll=%JAVA_HOME%\jre\bin\hotspot\jvm.dll
if not exist "%jvmdll%" SET jvmdll=%JAVA_HOME%\jre\bin\client\jvm.dll
if not exist "%jvmdll%" goto no_java
SET toolsjar=%JAVA_HOME%\lib\tools.jar
if not exist "%toolsjar%" goto no_java


rem check that JBoss exists and environment variable is set up
if "%JBOSS_HOME%" == "" goto no_jboss
if not exist "%JBOSS_HOME%\bin" goto no_jboss
SET jbossjar=%JBOSS_HOME%\bin\run.jar
if not exist "%jbossjar%" goto no_jboss


rem determine which optional service parameters may have been specified
SET svcmode=""
SET dependson=""
SET dependopt=""
if "%2" == "-manual" SET svcmode=-manual
if "%2" == "-auto" SET svcmode=-auto
if "%1" == "-manual" SET svcmode=-manual
if "%1" == "-auto" SET svcmode=-auto
if not "%svcmode%" == "" SET dependson=%1
if not "%dependson%" == "" SET dependopt=-depends %dependson%

rem parameters and files seem ok, go ahead with the service installation

@echo .


rem Copy executable to get informative process image name
SET jbossexe=%JBOSS_HOME%\bin\JBossService.exe
copy JavaService.exe "%jbossexe%" > nul

"%jbossexe%" -install JBoss "%jvmdll%" -Djava.class.path="%jbossjar%;%toolsjar%" -Xms64M -Xmx128M -start org.jboss.Main -stop org.jboss.Main -method systemExit -out "%JBOSS_HOME%\bin\out.log" -err "%JBOSS_HOME%\bin\err.log" -current "%JBOSS_HOME%\bin" %dependopt% %svcmode% -overwrite -startup 6

if ERRORLEVEL 1 goto js_error

goto end



:no_jsexe
@echo . JavaService executable file not found, installation script cannot be run
goto error_exit

:no_java
@echo . JavaService installation script requires the JAVA_HOME environment variable
@echo . The Java run-time files tools.jar and jvm.dll must exist under that location
goto error_exit

:no_jboss
@echo . JavaService installation script requires the JBOSS_HOME environment variable
@echo . The JBoss Application Server must exist in the bin sub-directory at that location
goto error_exit

:js_error
@echo . JavaService indicated an error in attempting to install the service
goto error_exit

:error_exit

@echo .
@echo . Failed to install JBoss as a system service
@echo .
@echo . Command format:
@echo .
@echo .  %~n0 [depends_on] [-auto / -manual]
@echo .
@echo . Where:
@echo .  depends_on is a service that must be running before JBoss starts
@echo .  -auto (default) or -manual flag can specify startup mode for the service
@echo .
@echo . Example:
@echo .  %~n0 mysql -auto


:end
ENDLOCAL
@echo .
@pause
