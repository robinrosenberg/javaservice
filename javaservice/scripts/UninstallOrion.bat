@echo off

rem * JavaService uninstall script for Orion Application Server
rem *
rem * JavaService - Windows NT Service Daemon for Java applications
rem * Copyright (C) 2004 Multiplan Consultants Ltd. LGPL Licensing applies
rem * Information about the JavaService software is available at the ObjectWeb
rem * web site. Refer to http://javaservice.objectweb.org for more details.

SETLOCAL

rem check that Orion directory is specified on this command
if "%1" == "" goto no_orion
rem verify that the Orion JavaService exe file is available
SET orionexe="%1\OrionService.exe"
if not exist "%orionexe%" goto no_jsexe


rem parameters and files seem ok, go ahead with the service uninstall

@echo .

"%orionexe%" -uninstall Orion
if ERRORLEVEL 1 goto js_error

del "%orionexe%"

goto end



:no_orion
@echo . JavaService uninstall script requires the Orion home directory as a parameter
goto error_exit

:no_jsexe
@echo . Orion JavaService executable file not found, uninstall script cannot be run
goto error_exit

:js_error
@echo . JavaService indicated an error in attempting to uninstall the service
goto error_exit

:error_exit

@echo .
@echo . Failed to uninstall Orion system service
@echo .


:end
ENDLOCAL
@echo .
@pause
