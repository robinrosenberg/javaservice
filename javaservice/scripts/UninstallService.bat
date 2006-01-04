@echo off

rem * JavaService uninstall script for Generic Java Service process
rem * Assumed to be used where exe file is a copy of Javaservice
rem * and is to be deleted after uninstall processing.
rem *
rem * Usage: UninstallService exe_name service_name
rem *
rem * JavaService - Windows NT Service Daemon for Java applications
rem * Copyright (C) 2006 Multiplan Consultants Ltd. LGPL Licensing applies
rem * Information about the JavaService software is available at the ObjectWeb
rem * web site. Refer to http://javaservice.objectweb.org for more details.

SETLOCAL

@echo .

rem check that copied JavaService executable is specified as a parameter
if "%1" == "" goto no_p1
rem verify that the specified JavaService exe file is available
SET jsexe=%1
rem allow for user entering filename without the .exe suffix
if exist "%jsexe%.exe" SET jsexe=%jsexe%.exe
if not exist "%jsexe%" goto no_jsexe

rem check that installed service name is also specified as a parameter
if "%2" == "" goto no_p2
SET svcname=%2

rem parameters seem ok, go ahead with the service uninstall

"%jsexe%" -uninstall %svcname%
if ERRORLEVEL 1 goto js_error

del "%jsexe%"
if ERRORLEVEL 1 goto del_error

goto end



:no_p1
@echo . Uninstall script requires the JavaService executable as first parameter
goto error_exit

:no_jsexe
@echo . JavaService executable file %jsexe% not found, uninstall script cannot be run
goto error_exit

:no_p2
@echo . Uninstall script requires the service name as second parameter
goto error_exit

:js_error
@echo . JavaService indicated an error in attempting to uninstall the service
goto error_exit

:del_error
@echo . Warning. Service uninstalled, but failed to delete executable file %jsexe%
goto error_exit

:error_exit

@echo .
@echo . Failed to uninstall %svcname% system service
@echo .


:end
ENDLOCAL
@echo .
@pause
