call installSampleService c:\j2sdk1.4.2\jre\bin\server c:\javaservice\dev\SampleService\bin c:\javaservice\dev\JavaService\bin -manual

net start SampleService
@pause

net stop SampleService
@pause

call uninstallSampleService c:\javaservice\dev\SampleService\bin
