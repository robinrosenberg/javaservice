@echo off

rem * JavaService installation script for Orion Application Server
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


rem check that Orion directory is specified correctly on this command
if "%1" == "" goto no_orion
SET orionjar=%2\orion.jar
if not exist "%orionjar%" goto no_orion

rem check that Orion administrator credentials supplied for this command
if "%2" == "" goto no_admin
if "%3" == "" goto no_admin


rem determine which optional service parameters may have been specified
SET svcmode=""
SET dependson=""
SET dependopt=""
if "%5" == "-manual" SET svcmode=-manual
if "%5" == "-auto" SET svcmode=-auto
if "%4" == "-manual" SET svcmode=-manual
if "%4" == "-auto" SET svcmode=-auto
if not "%svcmode%" == "" SET dependson=%4
if not "%dependson%" == "" SET dependopt=-depends %dependson%

rem parameters and files seem ok, go ahead with the service installation

@echo .


rem Copy executable to get informative process image name
SET orionexe=%1\OrionService.exe
copy JavaService.exe "%orionexe%" > nul


"%orionexe%" -install Orion "%jvmdll%" -Djava.class.path="%orionjar%;%toolsjar%" -Xms64M -Xmx128M -start com.evermind.server.ApplicationServer -stop com.evermind.client.orion.OrionConsoleAdmin -params "ormi://localhost %2 %3 -shutdown" -out "%1\log\stdout.log" -err "%1\log\stderr.log" -current "%1"  %dependopt% %svcmode%

if ERRORLEVEL 1 goto js_error

goto end



:no_jsexe
@echo . JavaService executable file not found, installation script cannot be run
goto error_exit

:no_java
@echo . JavaService installation script requires the JAVA_HOME environment variable
@echo . The Java run-time files tools.jar and jvm.dll must exist under that location
goto error_exit

:no_orion
@echo . JavaService installation script requires the Orion directory name
@echo . Specify the directory in which Orion is installed (no spaces on the path)
goto error_exit

:no_admin
@echo . JavaService installation script requires the Orion administrator credentials
@echo . These must be specified on the command line when installing the application
goto error_exit

:js_error
@echo . JavaService indicated an error in attempting to install the service
goto error_exit

:error_exit

@echo .
@echo . Failed to install Orion Application Server as a system service
@echo .
@echo . Command format:
@echo .
@echo .  %~n0 orion_home admin_user admin_pwd [depends_on] [-auto / -manual]
@echo .
@echo . Where:
@echo .  orion_home indicates the directory in which Orion is installed (no spaces)
@echo .  admin_user is the Orion administrator username
@echo .  admin_pwd is the Orion administrator password
@echo .  depends_on is a service that must be running before Orion starts
@echo .  -auto (default) or -manual flag can specify startup mode for the service
@echo .
@echo . Examples:
@echo .  %~n0 c:\orion orionadmin orionnimda mysql -auto
@echo .  %~n0 %%ORION_HOME%% orionadmin orionnimda mysql -auto


:end
ENDLOCAL
@echo .
@pause
