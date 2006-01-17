/*
 * JavaService - Windows NT Service Daemon for Java applications
 *
 * Copyright (C) 2006 Multiplan Consultants Ltd.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Information about the JavaService software is available at the ObjectWeb
 * web site. Refer to http://javaservice.objectweb.org for more details.
 *
 * This software is based on earlier work by Alexandria Software Consulting,
 * which was released under a BSD-style license in April 2001. Enhancements
 * were made to the V1.2.x releases under this license by Multiplan Consultants Ltd.
 * The V2.x software is a new development issued under the LGPL license alone.
 *
 */

#include <windows.h>
#include "jni.h"
#include "JavaInterface.h"
#include "ServiceHandler.h"
#include "ServiceLogger.h"
#include "EventLogger.h"
#include "Messages.h"
#include "ServiceParameters.h"

////
//// Local constant definitions
/////

static const char* SYSTEM_CLASS = "java.lang.System";
static const char* EXIT_METHOD = "exit";
static const int EXIT_STATUS = 1;


////
//// Local function prototypes
////

static bool StartJavaService(HANDLE hEventSource, const char *jvmDllPath, int jvmOptionCount, const char* jvmOptions[], const char *startClass, const char *startMethod, int startParamCount, const char *startParams[], const char *outFile, const char *errFile, bool overwriteFiles);
static bool StopJavaService(HANDLE hEventSource, const char *stopClass, const char *stopMethod, int stopParamCount, const char *stopParams[]);
static bool StopJVM(HANDLE hEventSource, int exitStatusCode);


static bool invokeClassMethod(HANDLE hEventSource, JNIEnv *env, const char* className, const char* methodName, int paramCount, const char** params);
static bool invokeExitMethod(HANDLE hEventSource, JNIEnv *env, int exitStatusCode);

static bool redirectSystemOut(HANDLE hEventSource, JNIEnv *env, const char *outFile, const bool overwriteMode);
static bool redirectSystemErr(HANDLE hEventSource, JNIEnv *env, const char *errFile, const bool overwriteMode);
static bool redirectStream(HANDLE hEventSource, JNIEnv *env, const char *streamFile, bool isStdout, const bool overwriteMode);

static bool exceptionRaised(JNIEnv *jniEnv);

static const char* getClassAsPath(const char* className);


//Type for JNI_CreateJavaVM function.
typedef jint (JNICALL *JNICREATEPROC)(JavaVM **, void **, void *);



////
//// Local variable definitions
////

//The Java Virtual Machine the service is running in.
static JavaVM *jvmInstance = NULL;


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
		ServiceLogger::write("ConsoleControlHandler detected control-type event\n");
        return TRUE;

	default:
		// Let parent handler (the VM) take care of anything else.
		ServiceLogger::write("ConsoleControlHandler detected/ignored event\n");
		return FALSE;
    }

}



//
// Global function invoked when JVM and configured class is to be invoked as a service thread
//
bool StartJavaService(HANDLE hEventSource, const ServiceParameters* serviceParams)
{

	ServiceLogger::write("StartJavaService(serviceParams)\n");
	bool started = StartJavaService(hEventSource,
									serviceParams->getJvmLibrary(),
									serviceParams->getJvmOptionCount(),
									serviceParams->getJvmOptions(),
									serviceParams->getStartClass(),
									serviceParams->getStartMethod(),
									serviceParams->getStartParamCount(),
									serviceParams->getStartParams(),
									serviceParams->getOutFile(),
									serviceParams->getErrFile(),
									serviceParams->getFileOverwriteFlag());

	ServiceLogger::write(started ? "StartJavaService successful\n" : "StartJavaService failed\n");
	return started;
}


static bool StartJavaService(HANDLE hEventSource, const char *jvmDllPath, int jvmOptionCount, const char* jvmOptions[], const char *startClass, const char *startMethod, int startParamCount, const char *startParams[], const char *outFile, const char *errFile, bool overwriteFiles)
{
	//Load a jvm DLL.
	HINSTANCE jvmDll = LoadLibrary(jvmDllPath);
	
	//Make sure we loaded a jvm DLL.
	if (jvmDll == NULL)
	{
		logFunctionError(hEventSource, "LoadLibrary");
		logError(hEventSource, "Could not load the Java Virtual Machine.");
		logError(hEventSource, jvmDllPath);
		return false;
	}
	
	//Find the JNI_CreateJavaVM precodure in the DLL.
	JNICREATEPROC jvmCreateProc = (JNICREATEPROC)GetProcAddress(jvmDll, "JNI_CreateJavaVM");
	if (jvmCreateProc == NULL)
	{
		logFunctionError(hEventSource, "GetProcAddress");
		logError(hEventSource, "Could not find the JNI_CreateJavaVM function in the Virtual Machine DLL.");

		FreeLibrary(jvmDll);
		return false;
	}

	//Create the jvm options.
	JavaVMOption *options = new JavaVMOption[jvmOptionCount+1];
	for (int i=0; i<jvmOptionCount; i++)
	{
		options[i].optionString = (char*)jvmOptions[i];
	}

	//Set the exit hook as an additional jvm option
	options[jvmOptionCount].optionString = "exit";
	options[jvmOptionCount].extraInfo = ExitHandler;

	//Create the jvm init arguments.
	JavaVMInitArgs vm_args;
	vm_args.version = JNI_VERSION_1_2;
	vm_args.options = options;
	vm_args.nOptions = jvmOptionCount+1;
	vm_args.ignoreUnrecognized = JNI_TRUE;

	//Create a jvm
    JNIEnv *env = NULL;
	jint r = (jvmCreateProc)(&jvmInstance, (void **)&env, &vm_args);

	//Free the options buffer, before checking for creation status
	delete[] options;

	if ((r < 0) || (jvmInstance == NULL) || (env == NULL))
	{
		logError(hEventSource, "Could not initialize the Java Virtual Machine.");

		FreeLibrary(jvmDll);
		return false;
	}

	ServiceLogger::write("Java Virtual Machine initialised, got env reference\n");

	//Set a handler for the console to override the default JVM behavior.
	SetConsoleCtrlHandler(ConsoleControlHandler, TRUE);

	//Redirect System.out, if necessary.
	if (outFile != NULL)
	{
		if (!redirectSystemOut(hEventSource, env, outFile, overwriteFiles))
		{
			FreeLibrary(jvmDll);
			return false;
		}
	}

	//Redirect System.err, if necessary.
	if (errFile != NULL)
	{
		if (!redirectSystemErr(hEventSource, env, errFile, overwriteFiles))
		{
			FreeLibrary(jvmDll);
			return false;
		}
	}

	ServiceLogger::write("Invoking start method:");
	ServiceLogger::write(startClass);
	ServiceLogger::write(".");
	ServiceLogger::write(startMethod);
	ServiceLogger::write("()\n");

	// Invoke the service start method
	if (!invokeClassMethod(hEventSource, env, startClass, startMethod, startParamCount, startParams))
	{
		logError(hEventSource, "Failed to invoke the service start function.");
		FreeLibrary(jvmDll);
		return false;
	}

	ServiceLogger::write("Start function invoked, application loaded\n");

	//Free the jvm DLL and return success.
	FreeLibrary(jvmDll);
	return true;
}


//
// Global function invoked when JVM thread is to be shut down
//
bool StopJavaService(HANDLE hEventSource, const ServiceParameters* serviceParams)
{
	ServiceLogger::write("StopJavaService(serviceParams)\n");
	return StopJavaService(hEventSource, 
							serviceParams->getStopClass(),
							serviceParams->getStopMethod(),
							serviceParams->getStopParamCount(),
							serviceParams->getStopParams());
}


static bool StopJavaService(HANDLE hEventSource, const char *stopClass, const char *stopMethod, int stopParamCount, const char *stopParams[])
{
    JNIEnv *env;
	JavaVMAttachArgs attach_args;

	attach_args.version = JNI_VERSION_1_2;
	attach_args.name = "Stop Thread";
	attach_args.group = NULL;

	jint ret = jvmInstance->AttachCurrentThread((void**)&env, (void*)&attach_args);
	if (ret != 0)
	{
		logError(hEventSource, "Could not attach the stop thread to the Java Virtual Machine.");
		return false;
	}
	if (exceptionRaised(env))
	{
		logError(hEventSource, "Exception after attaching to the Java Virtual Machine.");
		return false;
	}

	// Invoke the service stop method
	if (!invokeClassMethod(hEventSource, env, stopClass, stopMethod, stopParamCount, stopParams))
	{
		logError(hEventSource, "Failed to invoke the service stop function.");
		return false;
	}

	return true;
}

//
// Global function used to stop java machine execution
//
bool StopJavaMachine(HANDLE hEventSource)
{
	ServiceLogger::write("StopJavaMachine()\n");
	return StopJVM(hEventSource, EXIT_STATUS);
}


static bool StopJVM(HANDLE hEventSource, int exitStatusCode)
{
    JNIEnv *env;
	JavaVMAttachArgs attach_args;

	attach_args.version = JNI_VERSION_1_2;
	attach_args.name = "Exit Thread";
	attach_args.group = NULL;

	jint ret = jvmInstance->AttachCurrentThread((void**)&env, (void*)&attach_args);
	if (ret != 0)
	{
		logError(hEventSource, "Could not attach the exit thread to the Java Virtual Machine.");
		return false;
	}
	if (exceptionRaised(env))
	{
		logError(hEventSource, "Exception after attaching to the Java Virtual Machine to exit.");
		return false;
	}

	// Invoke the Java System.exit() method
	if (!invokeExitMethod(hEventSource, env, exitStatusCode))
	{
		logError(hEventSource, "Failed to invoke the system exit function.");
		return false;
	}

	return true;
}


static bool redirectSystemOut(HANDLE hEventSource, JNIEnv *env, const char *outFile, const bool overwriteMode)
{
	return redirectStream(hEventSource, env, outFile, true, overwriteMode);
}



static bool redirectSystemErr(HANDLE hEventSource, JNIEnv *env, const char *errFile, const bool overwriteMode)
{
	return redirectStream(hEventSource, env, errFile, false, overwriteMode);
}


static bool redirectStream(HANDLE hEventSource, JNIEnv *env, const char *streamFile, bool isStdout, bool overwriteMode)
{
	const char* wotStream = isStdout ? "System.out" : "System.err";

	//Create a String for the path.
	jstring pathString = env->NewStringUTF(streamFile);
	if (exceptionRaised(env) || (pathString == NULL))
	{
		logError(hEventSource, "Could not create a path string for ", wotStream);
		return false;
	}

	//Find the FileOutputStream class.
    jclass fileOutputStreamClass = env->FindClass("java/io/FileOutputStream");
    if (exceptionRaised(env) || (fileOutputStreamClass == NULL))
	{
		logError(hEventSource, "Could not find the FileOutputStream class for redirect of ", wotStream);
		return false;
    }

	// two forms of output stream constructor may be used
	// later (JDK1.4) version may be available to allow append mode on existing files
	// earlier version otherwise used will overwrite any existing files each time
	// parameter specifies whether overwrite mode is to be used in any case

	bool useAppendParam = false; // flag set if longer constructor is being used
	jmethodID fileOutputStreamConstructor = NULL; // reference to either ctor

	if (!overwriteMode)
	{
		// try to get reference to two-parameter JDK 1.4 constructor, allowing append mode
		fileOutputStreamConstructor = env->GetMethodID(fileOutputStreamClass, "<init>", "(Ljava/lang/String;Z)V");

		// ensure that exception check is included first here, to reset any error found
		if (exceptionRaised(env) || (fileOutputStreamConstructor == NULL))
		{
			useAppendParam = false; // could not found the desired long form of constructor
		}
	    else
		{
			useAppendParam = true; // found the later form of constructor, so use append param
		}
	}

	if (overwriteMode || !useAppendParam)
	{
		// either wanted overwrite mode, or could not get later constructor reference
		// get reference to single-parameter constructor, will overwrite any existing files

	    fileOutputStreamConstructor = env->GetMethodID(fileOutputStreamClass, "<init>", "(Ljava/lang/String;)V");
	}

    if (exceptionRaised(env) || (fileOutputStreamConstructor == NULL))
	{
		logError(hEventSource, "Could not find the FileOutputStream constructor for redirect of ", wotStream);
		return false;
    }

	//Create a FileOutputStream.
    jobject fileOutputStream = useAppendParam ? env->NewObject(fileOutputStreamClass, fileOutputStreamConstructor, pathString, JNI_TRUE)
											  :	env->NewObject(fileOutputStreamClass, fileOutputStreamConstructor, pathString);
    if (exceptionRaised(env) || (fileOutputStream == NULL))
	{
		logError(hEventSource, "Could not create a FileOutputStream for redirect of ", wotStream);
		return false;
    }

	//Find the PrintStream class.
    jclass printStreamClass = env->FindClass("java/io/PrintStream");
    if (exceptionRaised(env) || (printStreamClass == NULL))
	{
		logError(hEventSource, "Could not find the PrintStream class for redirect of ", wotStream);
		return false;
    }

	//Find the PrintStream constructor.
    jmethodID printStreamConstructor = env->GetMethodID(printStreamClass, "<init>", "(Ljava/io/OutputStream;)V");
    if (exceptionRaised(env) || (printStreamConstructor == NULL))
	{
		logError(hEventSource, "Could not find the PrintStream constructor for redirect of ", wotStream);
		return false;
    }

	//Create a PrintStream.
    jobject printStream = env->NewObject(printStreamClass, printStreamConstructor, fileOutputStream);
    if (exceptionRaised(env) || (printStream == NULL))
	{
		logError(hEventSource, "Could not create a PrintStream for redirect of ", wotStream);
		return false;
    }

	//Find the System class.
    jclass systemClass = env->FindClass("java/lang/System");
    if (exceptionRaised(env) || (systemClass == NULL))
	{
		logError(hEventSource, "Could not find the System class for redirect of ", wotStream);
		return false;
    }

	//Find the setOut/setErr() method.
    jmethodID setMethod = env->GetStaticMethodID(systemClass, isStdout ? "setOut" : "setErr", "(Ljava/io/PrintStream;)V");
    if (exceptionRaised(env) || (setMethod == NULL))
	{
		logError(hEventSource, "Could not find the setErr method for redirect of ", wotStream);
		return false;
    }

	//Call System.setOut/setErr().
	env->CallStaticVoidMethod(systemClass, setMethod, printStream);
	if (exceptionRaised(env))
	{
		logError(hEventSource, "Could not call the setErr method for redirect of ", wotStream);
		return false;
	}

	return true;
}



static bool invokeClassMethod(HANDLE hEventSource, JNIEnv *env, const char* className, const char* methodName, int paramCount, const char** params)
{

	ServiceLogger::write("Will invoke method ");
	ServiceLogger::write(methodName);
	if (paramCount > 0) {
		ServiceLogger::write(" with parameters:");
		for (int lp = 0; lp < paramCount; lp++)
		{
			if (lp > 0) ServiceLogger::write(", ");
			ServiceLogger::write(params[lp]);
		}
	}
	ServiceLogger::write("\n");

	// find the specified class, using slash-delimited directory style name, not dot-delimited class

	const char* classPath = getClassAsPath(className);
    jclass serviceClass = env->FindClass(classPath);
	delete[] (void*)classPath;

	if (exceptionRaised(env) || (serviceClass == NULL))
	{
		logError(hEventSource, "Could not find the service class.");
		return false;
    }
	ServiceLogger::write("Found service class\n");

	// find the specified service method, with same signature as a normal main method

    jmethodID mid = env->GetStaticMethodID(serviceClass, methodName, "([Ljava/lang/String;)V");
    if (exceptionRaised(env) || (mid == NULL))
	{
		logError(hEventSource, "Could not find the service method.");
		return false;
    }
	ServiceLogger::write("Found service method\n");

	// find the String class, for the parameter array

    jclass stringClass = env->FindClass("java/lang/String");
    if (exceptionRaised(env) || (stringClass == NULL))
	{
		logError(hEventSource, "Could not find the String class.");
		return false;
    }
	ServiceLogger::write("Found String class\n");

	// create a String array for the parameters

	jobjectArray parameterArray = env->NewObjectArray(paramCount, stringClass, NULL);
    if (exceptionRaised(env) || (parameterArray == NULL))
	{
		logError(hEventSource, "Could not create a string parameter array.");
		return false;
    }
	ServiceLogger::write("Created parameter array\n");

	// fill in the String array with the arguments

	for (int i = 0; i < paramCount; i++)
	{
		jstring parameterString = env->NewStringUTF(params[i]);
		if (exceptionRaised(env) || (parameterString == NULL))
		{
			logError(hEventSource, "Could not create a parameter string object.");
			return false;
		}

		env->SetObjectArrayElement(parameterArray, i, parameterString);
		if (exceptionRaised(env))
		{
			logError(hEventSource, "Could not set an element in the string parameter array.");
			return false;
		}
	}
	ServiceLogger::write("Filled parameter array\n");

	// now call the service method with it's specified parameters

	ServiceLogger::write("Invoking static method now\n");
	env->CallStaticVoidMethod(serviceClass, mid, parameterArray);
	if (exceptionRaised(env))
	{
		logError(hEventSource, "Could not call the service method.");
		return false;
	}

	ServiceLogger::write("Static method completed ok\n");

	return true; // successfully invoked the method

}


static bool invokeExitMethod(HANDLE hEventSource, JNIEnv *env, int exitStatusCode)
{

	ServiceLogger::write("Will invoke System.exit method with success status code\n");

	// find the java.lang.System class, using slash-delimited directory style name, not dot-delimited class

	const char* classPath = getClassAsPath(SYSTEM_CLASS);
    jclass systemClass = env->FindClass(classPath);
	delete[] (void*)classPath;

	if (exceptionRaised(env) || (systemClass == NULL))
	{
		logError(hEventSource, "Could not find the System class.");
		return false;
    }
	ServiceLogger::write("Found System class\n");

	// find the specified service method, with single-int parameter signature

    jmethodID mid = env->GetStaticMethodID(systemClass, EXIT_METHOD, "(I)V");
    if (exceptionRaised(env) || (mid == NULL))
	{
		logError(hEventSource, "Could not find the System.exit method.");
		return false;
    }
	ServiceLogger::write("Found System.exit method\n");

	// now call the exit method with it's single parameter

	jint exitStatus = exitStatusCode;
	ServiceLogger::write("Invoking static exit method now\n");
	env->CallStaticVoidMethod(systemClass, mid, exitStatus); // invokes exit handler(s)
	if (exceptionRaised(env))
	{
		logError(hEventSource, "Could not call the System.exit method.");
		return false;
	}

	ServiceLogger::write("System.exit method completed ok\n");

	return true; // successfully invoked the System.exit method, assume JVM dead/dying now (with exit handlers)

}




// convert from dot-delimited class name to slash-delimited directory style
static const char* getClassAsPath(const char* className)
{
	ServiceLogger::write("Getting classname as a path\n");

	if (className == NULL)
	{
		ServiceLogger::write("NULL classname specified, returning NULL path\n");
		return NULL;
	}

	ServiceLogger::write("Processing class name '");
	ServiceLogger::write(className);
	ServiceLogger::write("'\n");

	int nameLen = strlen(className);

	char* classPath = new char[nameLen + 1];
	memset(classPath, 0, nameLen + 1);
	for (int i = 0; i <= nameLen; i++)
	{
		const char chr = className[i];

		if (chr == '.')
		{
			classPath[i] = '/'; // note, windows-style directory delimiter, not unix-style
		}
		else
		{
			classPath[i] = chr;
		}
	}

	ServiceLogger::write("Generated class path '");
	ServiceLogger::write(classPath);
	ServiceLogger::write("'\n");

	return classPath; // caller needs to delete this
}




static bool exceptionRaised(JNIEnv *jniEnv)
{
	bool wasAnException = (jniEnv->ExceptionCheck() == JNI_TRUE);

	if (wasAnException)
	{
		ServiceLogger::write("Java run-time exception was generated\n");
		jniEnv->ExceptionDescribe();
		jniEnv->ExceptionClear();
	}

	return wasAnException;
}
