
//Function to start a java class as a service.
bool StartJavaService(HANDLE hEventSource, char *jvmDllPath, int jvmOptionCount, char* jvmOptions[], char *startClass, char *startMethod, int startParamCount, char *startParams[], char *outFile, char *errFile, bool overwriteFiles);
bool StopJavaService(HANDLE hEventSource, char *stopClass, char *stopMethod, int stopParamCount, char *stopParams[]);
