//Copyright (c) 2000, Alexandria Software Consulting
//Some enhancements, post V1.2.0, (c) 2005 Multiplan Consultants Ltd
//
//All rights reserved. Redistribution and use in source 
//and binary forms, with or without modification, are permitted provided 
//that the following conditions are met:
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions, and the following disclaimer.
// - Neither name of Alexandria Software Consulting nor the names of the 
//   contributors may be used to endorse or promote products derived from 
//   this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS 
//AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
//INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
//AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
//THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
//INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
//NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
//EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <windows.h>
#include "jni.h"
#include "JavaInterface.h"
#include "ServiceInterface.h"
#include "Messages.h"

//Type for JNI_CreateJavaVM function.
typedef jint (JNICALL *JNICREATEPROC)(JavaVM **, void **, void *);

////
//// Local function prototypes
////

//Methods for redirecting out and err.
static bool redirectSystemOut(HANDLE hEventSource, JNIEnv *env, char *outFile);
static bool redirectSystemErr(HANDLE hEventSource, JNIEnv *env, char *errFile);

////
//// Local variable definitions
////

//The Java Virtual Machine the service is running in.
static JavaVM *jvm = NULL;


//
// Hook function for when control-key type events are delivered to the process
//
static BOOL WINAPI ConsoleControlHandler(DWORD dwCtrlType)
{
    switch (dwCtrlType)
	{
    case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    case CTRL_LOGOFF_EVENT:
        return TRUE;
    }

    // Let parent handler (the VM) take care of anything else.
    return FALSE;
}



//
// Global function invoked when JVM and configured class is to be invoked as a service thread
//
bool StartJavaService(HANDLE hEventSource, char *jvmDllPath, int jvmOptionCount, char* jvmOptions[], char *startClass, char *startMethod, int startParamCount, char *startParams[], char *outFile, char *errFile)
{
	//Load a jvm DLL.
	HINSTANCE jvmDll = LoadLibrary(jvmDllPath);
	
	//Make sure we loaded a jvm DLL.
	if (jvmDll == NULL)
	{
		LPTSTR messages[2];
		messages[0] = "LoadLibrary";
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&messages[1], 0, NULL);
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_FUNCTION_FAILED, NULL, 2, 0, (const char **)messages, NULL);
		LocalFree(messages[1]);
		messages[0] = "Could not load the Java Virtual Machine.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
	}
	
	//Find the JNI_CreateJavaVM precodure in the DLL.
	JNICREATEPROC jvmCreateProc;
	if ((jvmCreateProc = (JNICREATEPROC)GetProcAddress(jvmDll, "JNI_CreateJavaVM")) == NULL)
	{
		LPTSTR messages[2];
		messages[0] = "GetProcAddress";
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&messages[1], 0, NULL);
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_FUNCTION_FAILED, NULL, 2, 0, (const char **)messages, NULL);
		LocalFree(messages[1]);
		messages[0] = "Could not find the JNI_CreateJavaVM function in the Virtual Machine.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		FreeLibrary(jvmDll);
		return false;
	}

	//Create the jvm options.
	JavaVMOption *options = new JavaVMOption[jvmOptionCount+1];
	for (int i=0; i<jvmOptionCount; i++)
	{
		options[i].optionString = jvmOptions[i];
	}

	//Set the exit hook.
	options[jvmOptionCount].optionString = "exit";
	options[jvmOptionCount].extraInfo = ExitHandler;

	//Create the jvm init arguments.
	JavaVMInitArgs vm_args;
	vm_args.version = JNI_VERSION_1_2;
	vm_args.options = options;
	vm_args.nOptions = jvmOptionCount+1;
	vm_args.ignoreUnrecognized = JNI_TRUE;

	//Create a jvm
    JNIEnv *env;
	jint r = (jvmCreateProc)(&jvm, (void **)&env, &vm_args);
	if (r < 0 || jvm == NULL || env == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not initialize the Java Virtual Machine.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		delete[] options;
		FreeLibrary(jvmDll);
		return false;
	}

	//Free the options buffer.
	delete[] options;

	//Set a handler for the console to override the default JVM behavior.
	SetConsoleCtrlHandler(ConsoleControlHandler, TRUE);

	//Redirect System.out, if necessary.
	if (outFile != NULL)
	{
		if (!redirectSystemOut(hEventSource, env, outFile))
		{
			FreeLibrary(jvmDll);
			return false;
		}
	}

	//Redirect System.err, if necessary.
	if (errFile != NULL)
	{
		if (!redirectSystemErr(hEventSource, env, errFile))
		{
			FreeLibrary(jvmDll);
			return false;
		}
	}

	//Load the service start class.
    jclass serviceClass = env->FindClass(startClass);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the service start class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		FreeLibrary(jvmDll);
		return false;
	}
    if (serviceClass == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the service start class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		FreeLibrary(jvmDll);
		return false;
    }
 
	//Find the service start method.
    jmethodID mid = env->GetStaticMethodID(serviceClass, startMethod, "([Ljava/lang/String;)V");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the start method.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		FreeLibrary(jvmDll);
		return false;
	}
    if (mid == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the start method.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		FreeLibrary(jvmDll);
		return false;
    }

	//Find the String class.
    jclass stringClass = env->FindClass("java/lang/String");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the String class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		FreeLibrary(jvmDll);
		return false;
	}
    if (stringClass == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the String class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		FreeLibrary(jvmDll);
		return false;
    }

	//Create a String array for the parameters.
	jobjectArray parameterArray = env->NewObjectArray(startParamCount, stringClass, NULL);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a start parameter array.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		FreeLibrary(jvmDll);
		return false;
	}
    if (parameterArray == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a start parameter array.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		FreeLibrary(jvmDll);
		return false;
    }

	//Fill in the String array with the arguments.
	for (i=0; i<startParamCount; i++)
	{
		jstring parameterString = env->NewStringUTF(startParams[i]);
		if (env->ExceptionCheck() == JNI_TRUE)
		{
			LPTSTR messages[1];
			messages[0] = "Could not create a start parameter string.";
			ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
			env->ExceptionDescribe();
			env->ExceptionClear();
			FreeLibrary(jvmDll);
			return false;
		}
		if (parameterString == NULL)
		{
			LPTSTR messages[1];
			messages[0] = "Could not create a start parameter string.";
			ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
			FreeLibrary(jvmDll);
			return false;
		}

		env->SetObjectArrayElement(parameterArray, i, parameterString);
		if (env->ExceptionCheck() == JNI_TRUE)
		{
			LPTSTR messages[1];
			messages[0] = "Could not set an element in the start parameter array.";
			ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
			env->ExceptionDescribe();
			env->ExceptionClear();
			FreeLibrary(jvmDll);
			return false;
		}
	}

	//Call the service method.
	env->CallStaticVoidMethod(serviceClass, mid, parameterArray);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not call the start method.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		FreeLibrary(jvmDll);
		return false;
	}

	//Free the jvm DLL and return success.
	FreeLibrary(jvmDll);
	return true;
}


static bool exceptionRaised(JNIEnv *env)
{
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		env->ExceptionDescribe();
		env->ExceptionClear();

		return true;
	}
	else
	{
		return false;
	}
}


static bool redirectSystemOut(HANDLE hEventSource, JNIEnv *env, char *outFile)
{
	//Create a String for the path.
	jstring pathString = env->NewStringUTF(outFile);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a string for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
	if (pathString == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a string for System.out path.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
	}

	//Find the FileOutputStream class.
    jclass fileOutputStreamClass = env->FindClass("java/io/FileOutputStream");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the FileOutputStream class for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (fileOutputStreamClass == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the FileOutputStream class for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }


	//Find the FileOutputStream constructor - look for JDK1.4 version first, of (String name, boolean append) format
	bool useAppendParam = true;
    jmethodID fileOutputStreamConstructor = env->GetMethodID(fileOutputStreamClass, "<init>", "(Ljava/lang/String;Z)V");
	// if this form of constructor not found (pre 1.4 JVM) then use simpler (String name) form instead, no append
    if (exceptionRaised(env) || (fileOutputStreamConstructor == NULL))
	{
		useAppendParam = false;
	    fileOutputStreamConstructor = env->GetMethodID(fileOutputStreamClass, "<init>", "(Ljava/lang/String;)V");
	}
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the FileOutputStream constructor for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (fileOutputStreamConstructor == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the FileOutputStream constructor for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Create a FileOutputStream.
    jobject fileOutputStream = useAppendParam ? env->NewObject(fileOutputStreamClass, fileOutputStreamConstructor, pathString, JNI_TRUE)
											  :	env->NewObject(fileOutputStreamClass, fileOutputStreamConstructor, pathString);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a FileOutputStream for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (fileOutputStream == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a FileOutputStream for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Find the PrintStream class.
    jclass printStreamClass = env->FindClass("java/io/PrintStream");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the PrintStream class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (printStreamClass == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the PrintStream class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Find the PrintStream constructor.
    jmethodID printStreamConstructor = env->GetMethodID(printStreamClass, "<init>", "(Ljava/io/OutputStream;)V");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the PrintStream constructor for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (printStreamConstructor == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the PrintStream constructor for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Create a PrintStream.
    jobject printStream = env->NewObject(printStreamClass, printStreamConstructor, fileOutputStream);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a PrintStream for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (printStream == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a PrintStream for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Find the System class.
    jclass systemClass = env->FindClass("java/lang/System");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the System class for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (systemClass == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the System class for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Find the setOut() method.
    jmethodID setOutMethod = env->GetStaticMethodID(systemClass, "setOut", "(Ljava/io/PrintStream;)V");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the setOut method for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (setOutMethod == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the setOut method for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Call System.setOut().
	env->CallStaticVoidMethod(systemClass, setOutMethod, printStream);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not call the setOut method for System.out redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}

	return true;
}



static bool redirectSystemErr(HANDLE hEventSource, JNIEnv *env, char *errFile)
{
	//Create a String for the path.
	jstring pathString = env->NewStringUTF(errFile);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a string for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
	if (pathString == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a string for System.err path.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
	}

	//Find the FileOutputStream class.
    jclass fileOutputStreamClass = env->FindClass("java/io/FileOutputStream");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the FileOutputStream class for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (fileOutputStreamClass == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the FileOutputStream class for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Find the FileOutputStream constructor - look for JDK1.4 version first, of (String name, boolean append) format
	bool useAppendParam = true;
    jmethodID fileOutputStreamConstructor = env->GetMethodID(fileOutputStreamClass, "<init>", "(Ljava/lang/String;Z)V");
	// if this form of constructor not found (pre 1.4 JVM) then use simpler (String name) form instead, no append
    if (exceptionRaised(env) || (fileOutputStreamConstructor == NULL))
	{
		useAppendParam = false;
	    fileOutputStreamConstructor = env->GetMethodID(fileOutputStreamClass, "<init>", "(Ljava/lang/String;)V");
	}
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the FileOutputStream constructor for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (fileOutputStreamConstructor == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the FileOutputStream constructor for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Create a FileOutputStream.
    jobject fileOutputStream = useAppendParam ? env->NewObject(fileOutputStreamClass, fileOutputStreamConstructor, pathString, JNI_TRUE)
											  :	env->NewObject(fileOutputStreamClass, fileOutputStreamConstructor, pathString);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a FileOutputStream for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (fileOutputStream == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a FileOutputStream for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Find the PrintStream class.
    jclass printStreamClass = env->FindClass("java/io/PrintStream");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the PrintStream class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (printStreamClass == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the PrintStream class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Find the PrintStream constructor.
    jmethodID printStreamConstructor = env->GetMethodID(printStreamClass, "<init>", "(Ljava/io/OutputStream;)V");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the PrintStream constructor for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (printStreamConstructor == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the PrintStream constructor for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Create a PrintStream.
    jobject printStream = env->NewObject(printStreamClass, printStreamConstructor, fileOutputStream);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a PrintStream for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (printStream == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a PrintStream for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Find the System class.
    jclass systemClass = env->FindClass("java/lang/System");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the System class for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (systemClass == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the System class for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Find the setErr() method.
    jmethodID setErrMethod = env->GetStaticMethodID(systemClass, "setErr", "(Ljava/io/PrintStream;)V");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the setErr method for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (setErrMethod == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the setErr method for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Call System.setErr().
	env->CallStaticVoidMethod(systemClass, setErrMethod, printStream);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not call the setErr method for System.err redirect.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}

	return true;
}


//
// Global function invoked when JVM thread is to be shut down
//
bool StopJavaService(HANDLE hEventSource, char *stopClass, char *stopMethod, int stopParamCount, char *stopParams[])
{
    JNIEnv *env;
	JavaVMAttachArgs attach_args;

	attach_args.version = JNI_VERSION_1_2;
	attach_args.name = "Stop Thread";
	attach_args.group = NULL;

	jint ret = jvm->AttachCurrentThread((void**)&env, (void*)&attach_args);
	if (ret != 0)
	{
		LPTSTR messages[1];
		messages[0] = "Could not attach the stop thread to the Java Virtual Machine.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
	}
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Exception after attaching to the Java Virtual Machine.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}

	//Load the service stop class.
    jclass serviceClass = env->FindClass(stopClass);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the service stop class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (serviceClass == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the service stop class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }
 
	//Find the service stop method.
    jmethodID mid = env->GetStaticMethodID(serviceClass, stopMethod, "([Ljava/lang/String;)V");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the stop method.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (mid == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the stop method.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Find the String class.
    jclass stringClass = env->FindClass("java/lang/String");
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the String class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (stringClass == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not find the String class.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Create a String array for the parameters.
	jobjectArray parameterArray = env->NewObjectArray(stopParamCount, stringClass, NULL);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a stop parameter array.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
    if (parameterArray == NULL)
	{
		LPTSTR messages[1];
		messages[0] = "Could not create a stop parameter array.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		return false;
    }

	//Fill in the String array with the arguments.
	for (int i=0; i<stopParamCount; i++)
	{
		jstring parameterString = env->NewStringUTF(stopParams[i]);
		if (env->ExceptionCheck() == JNI_TRUE)
		{
			LPTSTR messages[1];
			messages[0] = "Could not create a stop parameter string.";
			ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
			env->ExceptionDescribe();
			env->ExceptionClear();
			return false;
		}
		if (parameterString == NULL)
		{
			LPTSTR messages[1];
			messages[0] = "Could not create a stop parameter string.";
			ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
			return false;
		}

		env->SetObjectArrayElement(parameterArray, i, parameterString);
		if (env->ExceptionCheck() == JNI_TRUE)
		{
			LPTSTR messages[1];
			messages[0] = "Could not set an element in the stop parameter array.";
			ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
			env->ExceptionDescribe();
			env->ExceptionClear();
			return false;
		}
	}

	//Call the service stop method.
	env->CallStaticVoidMethod(serviceClass, mid, parameterArray);
	if (env->ExceptionCheck() == JNI_TRUE)
	{
		LPTSTR messages[1];
		messages[0] = "Could not call the stop method.";
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_GENERIC_ERROR, NULL, 1, 0, (const char **)messages, NULL);
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}

	return true;
}
