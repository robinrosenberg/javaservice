@echo off

rem * JavaService uninstall script for Tomcat Application Server
rem *
rem * JavaService - Windows NT Service Daemon for Java applications
rem * Copyright (C) 2006 Multiplan Consultants Ltd. LGPL Licensing applies
rem * Information about the JavaService software is available at the ObjectWeb
rem * web site. Refer to http://javaservice.objectweb.org for more details.

SETLOCAL

rem check that Tomcat home directory is specified on this command
if "%1" == "" goto no_tomcat
rem verify that the Tomcat JavaService exe file is available
SET tomcatexe=%1\bin\TomcatService.exe
if not exist "%tomcatexe%" SET tomcatexe=%1\TomcatService.exe
if not exist "%tomcatexe%" goto no_jsexe


rem parameters and files seem ok, go ahead with the service uninstall

@echo .

"%tomcatexe%" -uninstall Tomcat
if ERRORLEVEL 1 goto js_error

del "%tomcatexe%"
if ERRORLEVEL 1 goto del_error

goto end



:no_tomcat
@echo . JavaService uninstall script requires the Tomcat home directory as a parameter
goto error_exit

:no_jsexe
@echo . Tomcat JavaService executable file not found, uninstall script cannot be run
goto error_exit

:js_error
@echo . JavaService indicated an error in attempting to uninstall the service
goto error_exit

:del_error
@echo . Warning. Service uninstalled, but failed to delete executable file %tomcatexe%
goto error_exit

:error_exit

@echo .
@echo . Failed to uninstall Tomcat system service
@echo .


:end
ENDLOCAL
@echo .
@pause
