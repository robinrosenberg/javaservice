@echo off
if "%1" == "" goto nodir
cd %1
:nodir
@echo on
SampleService.exe -uninstall SampleService
del SampleService.exe
@pause
