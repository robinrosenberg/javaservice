@echo off
echo --------
echo Usage:   %0 jdk_home tomcat_home (classic/hotspot/server)
echo NOTE:    You MAY NOT use spaces in the path names. If you know how
echo          to fix this, please tell me.
echo          JDK 1.3 does not come with hotpot server by default, you must
echo          install this seperately if you wish to use it.
echo Example: %0 c:\progra~1\jdk c:\progra~1\tomcat hotspot
echo --------

if "%1" == "" goto eof
if "%2" == "" goto eof
if "%3" == "" goto eof

copy JavaService.exe %2\bin\Tomcat.exe > nul
%2\bin\Tomcat.exe -install Tomcat %1\jre\bin\%3\jvm.dll -Djava.class.path=%2\lib\webserver.jar;%2\lib\jasper.jar;%2\lib\jaxp.jar;%2\lib\parser.jar;%2\lib\ant.jar;%2\lib\servlet.jar;%1\lib\tools.jar -Dtomcat.home=%2 -start org.apache.tomcat.startup.Tomcat -params -config %2\conf\server.xml -stop org.apache.tomcat.startup.Tomcat -params -stop -config %2\conf\server.xml -out %2\logs\stdout.log -err %2\logs\stderr.log

goto eof

:eof
