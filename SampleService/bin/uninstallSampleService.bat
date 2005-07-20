@echo off
setlocal
@rem if parameter specified, go to that directory for the uninstall/delete operation
if "%1" == "" goto nopush
set currentdir=%CD%
cd %1
:nopush
SampleService.exe -uninstall SampleService
del SampleService.exe
@pause
if "%currentdir%" == "" goto nopop
cd %currentdir%
:nopop
