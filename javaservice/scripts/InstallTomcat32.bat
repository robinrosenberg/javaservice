@echo off

rem * JavaService installation script for Tomcat Application Server
rem *
rem * JavaService - Windows NT Service Daemon for Java applications
rem * Copyright (C) 2004 Multiplan Consultants Ltd. LGPL Licensing applies
rem * Information about the JavaService software is available at the ObjectWeb
rem * web site. Refer to http://javaservice.objectweb.org for more details.

if "%1" == "" goto err
if "%2" == "" goto err

copy JavaService.exe %2\bin\TomcatService.exe > nul
%2\bin\TomcatService.exe -install Tomcat %1\jre\bin\server\jvm.dll -Djava.class.path=%2\lib\webserver.jar;%2\lib\jasper.jar;%2\lib\jaxp.jar;%2\lib\parser.jar;%2\lib\ant.jar;%2\lib\servlet.jar;%1\lib\tools.jar -Dtomcat.home=%2 -start org.apache.tomcat.startup.Tomcat -params -config %2\conf\server.xml -stop org.apache.tomcat.startup.Tomcat -params -stop -config %2\conf\server.xml -out %2\logs\stdout.log -err %2\logs\stderr.log

goto eof

:err
@echo --------
@echo Usage:   %0 jdk_home tomcat_home
@echo Example: %0 c:\progra~1\jdk c:\progra~1\tomcat
@echo Note: do not include spaces in directory paths
@echo --------

:eof
