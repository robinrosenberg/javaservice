call installSampleService g:\j2sdk1.4.2_06\jre\bin\server d:\javaservice\dev\SampleService\bin d:\javaservice\dev\JavaService\bin -manual

net start SampleService
@pause

net stop SampleService
@pause

call uninstallSampleService d:\javaservice\dev\SampleService\bin
