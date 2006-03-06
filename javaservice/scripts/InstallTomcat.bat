@echo off

rem * JavaService installation script for Tomcat Application Server
rem *
rem * JavaService - Windows NT Service Daemon for Java applications
rem * Copyright (C) 2006 Multiplan Consultants Ltd. LGPL Licensing applies
rem * Information about the JavaService software is available at the ObjectWeb
rem * web site. Refer to http://javaservice.objectweb.org for more details.


SETLOCAL

rem verify that the JavaService exe file is available
if not exist "JavaService.exe" goto no_jsexe

rem check that Tomcat version parameter is specified and recognised
if "%1" == "" goto no_vers
if "%1" == "4" SET TC_VERS=4
if "%1" == "32" SET TC_VERS=32
if "%1" == "31" SET TC_VERS=31
if "%TC_VERS%" == "" goto no_vers


rem check that Java is installed and parameter is supplied
if "%2" == "" goto no_java
SET JAVA_HOME=%2
if not exist "%JAVA_HOME%\jre" goto no_java

rem check for any of server, hotspot or client Java run-times
SET jvmdll=%JAVA_HOME%\jre\bin\server\jvm.dll
if not exist "%jvmdll%" SET jvmdll=%JAVA_HOME%\jre\bin\hotspot\jvm.dll
if not exist "%jvmdll%" SET jvmdll=%JAVA_HOME%\jre\bin\client\jvm.dll
if not exist "%jvmdll%" goto no_java
SET toolsjar=%JAVA_HOME%\lib\tools.jar
if not exist "%toolsjar%" goto no_java


rem check that Tomcat exists and parameter is specified
if "%2" == "" goto no_tomcat
SET TC_HOME=%2
SET TC_BIN=%2\bin
if not exist "%TC_BIN%" goto no_tomcat

rem set up version-specific values for Tomcat install
goto tcv%TC_VERS%

:tcv4
SET TC_JARS=%TC_BIN%\bootstrap.jar;%TC_BIN%\servlet.jar
SET TC_DEF=-Dcatalina.home=%TC_HOME%
SET TC_START=org.apache.catalina.startup.Bootstrap -params start
SET TC_STOP=org.apache.catalina.startup.Bootstrap -params stop
goto versok

:tcv32
SET TC_LIB=%TC_HOME%\lib
if not exist "%TC_LIB%" goto no_tomcat

SET TC_XML=%TC_HOME%\conf\server.xml
if not exist "%TC_XML%" goto no_tomcat

SET TC_JARS=%TC_LIB%\webserver.jar;%TC_LIB%\jasper.jar;%TC_LIB%\jaxp.jar;%TC_LIB%\parser.jar;%TC_LIB%\ant.jar;%TC_LIB%\servlet.jar
SET TC_DEF=-Dtomcat.home=%TC_HOME%
SET TC_START=org.apache.tomcat.startup.Tomcat -params -config %TC_XML%
SET TC_STOP=org.apache.tomcat.startup.Tomcat -params -stop -config %TC_XML%
goto versok

:tcv31
SET TC_LIB=%TC_HOME%\lib
if not exist "%TC_LIB%" goto no_tomcat

SET TC_JARS=%TC_LIB%\webserver.jar;%TC_LIB%\jasper.jar;%TC_LIB%\xml.jar;%TC_LIB%\servlet.jar
SET TC_DEF=-Dtomcat.home=%TC_HOME%
SET TC_START=org.apache.tomcat.startup.Tomcat -params -start
SET TC_STOP=org.apache.tomcat.startup.Tomcat -params -stop
goto versok


:versok
rem see if optional service mode parameter has been specified
SET svcmode=
if "%4" == "-manual" SET svcmode=-manual
if "%4" == "-auto" SET svcmode=-auto

rem parameters and files seem ok, go ahead with the service installation

@echo .


rem Copy executable to get informative process image name
SET tcexe=%TC_BIN%\TomcatService.exe
copy JavaService.exe "%tcexe%" > nul


%tcexe% -install Tomcat "%jvmdll%" -Djava.class.path="%TC_JARS%;%toolsjar%" %TC_DEF% -start %TC_START% -stop %TC_STOP% -out "%TC_HOME%\logs\stdout.log" -err "%TC_HOME%\logs\stderr.log" %svcmode%


if ERRORLEVEL 1 goto js_error

goto end



:no_jsexe
@echo . JavaService executable file not found, installation script cannot be run
goto error_exit

:no_java
@echo . This install script requires the parameter to specify Java location
@echo . The Java run-time files tools.jar and jvm.dll must exist under that location
goto error_exit

:no_vers
@echo . This install script requires the parameter to specify Tomcat version (4/32/31)
goto error_exit

:no_tomcat
@echo . This install script requires thecparameter to specify Tomcat location
@echo . The Tomcat server files must exist in the bin sub-directory at that location
goto error_exit

:js_error
@echo . JavaService indicated an error in attempting to install the service
goto error_exit

:error_exit

@echo .
@echo . Failed to install Tomcat as a system service
@echo .
@echo . Command format:
@echo .
@echo .  %~n0 tomcat_version jdk_home tomcat_home [-auto / -manual]
@echo .
@echo . Where:
@echo . tomcatversion is 4, 32 or 31 (used to determine classpath, etc)
@echo .  -auto (default) or -manual flag can specify startup mode for the service
@echo .
@echo . Example:
@echo .  %~n0 4 c:\progra~1\jdk c:\progra~1\tomcat -auto


:end
ENDLOCAL
@echo .
@pause
