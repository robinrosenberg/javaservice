/*
 * JavaService - Windows NT Service Daemon for Java applications
 *
 * Copyright (C) 2004 Multiplan Consultants Ltd.
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
//// Local function prototypes
////

static bool StartJavaService(HANDLE hEventSource, const char *jvmDllPath, int jvmOptionCount, const char* jvmOptions[], const char *startClass, const char *startMethod, int startParamCount, const char *startParams[], const char *outFile, const char *errFile);
static bool StopJavaService(HANDLE hEventSource, const char *stopClass, const char *stopMethod, int stopParamCount, const char *stopParams[]);


static bool invokeClassMethod(HANDLE hEventSource, JNIEnv *env, const char* className, const char* methodName, int paramCount, const char** params);

static bool redirectSystemOut(HANDLE hEventSource, JNIEnv *env, const char *outFile);
static bool redirectSystemErr(HANDLE hEventSource, JNIEnv *env, const char *errFile);
static bool redirectStream(HANDLE hEventSource, JNIEnv *env, const char *streamFile, bool isStdout);

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
									serviceParams->jvmLibrary,
									serviceParams->jvmOptionCount,
									serviceParams->jvmOptions,
									serviceParams->startClass,
									serviceParams->startMethod,
									serviceParams->startParamCount,
									serviceParams->startParams,
									serviceParams->outFile,
									serviceParams->errFile);

	ServiceLogger::write(started ? "StartJavaService successful\n" : "StartJavaService failed\n");
	return started;
}


static bool StartJavaService(HANDLE hEventSource, const char *jvmDllPath, int jvmOptionCount, const char* jvmOptions[], const char *startClass, const char *startMethod, int startParamCount, const char *startParams[], const char *outFile, const char *errFile)
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
							serviceParams->stopClass,
							serviceParams->stopMethod,
							serviceParams->stopParamCount,
							serviceParams->stopParams);
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


static bool redirectSystemOut(HANDLE hEventSource, JNIEnv *env, const char *outFile)
{
	return redirectStream(hEventSource, env, outFile, true);
}



static bool redirectSystemErr(HANDLE hEventSource, JNIEnv *env, const char *errFile)
{
	return redirectStream(hEventSource, env, errFile, false);
}


static bool redirectStream(HANDLE hEventSource, JNIEnv *env, const char *streamFile, bool isStdout)
{
	//Create a String for the path.
	jstring pathString = env->NewStringUTF(streamFile);
	if (exceptionRaised(env) || (pathString == NULL))
	{
		logError(hEventSource, isStdout ? "Could not create a string for System.out path."
										: "Could not create a string for System.err path.");
		return false;
	}

	//Find the FileOutputStream class.
    jclass fileOutputStreamClass = env->FindClass("java/io/FileOutputStream");
    if (exceptionRaised(env) || (fileOutputStreamClass == NULL))
	{
		logError(hEventSource, isStdout ? "Could not find the FileOutputStream class for System.out redirect."
										: "Could not find the FileOutputStream class for System.err redirect.");
		return false;
    }

	//Find the FileOutputStream constructor.
    jmethodID fileOutputStreamConstructor = env->GetMethodID(fileOutputStreamClass, "<init>", "(Ljava/lang/String;)V");
    if (exceptionRaised(env) || (fileOutputStreamConstructor == NULL))
	{
		logError(hEventSource, isStdout ? "Could not find the FileOutputStream constructor for System.out redirect."
										: "Could not find the FileOutputStream constructor for System.err redirect.");
		return false;
    }

	//Create a FileOutputStream.
    jobject fileOutputStream = env->NewObject(fileOutputStreamClass, fileOutputStreamConstructor, pathString);
    if (exceptionRaised(env) || (fileOutputStream == NULL))
	{
		logError(hEventSource, isStdout ? "Could not create a FileOutputStream for System.out redirect."
										: "Could not create a FileOutputStream for System.err redirect.");
		return false;
    }

	//Find the PrintStream class.
    jclass printStreamClass = env->FindClass("java/io/PrintStream");
    if (exceptionRaised(env) || (printStreamClass == NULL))
	{
		logError(hEventSource, isStdout ? "Could not find the PrintStream class for System.out redirect."
										: "Could not find the PrintStream class for System.err redirect.");
		return false;
    }

	//Find the PrintStream constructor.
    jmethodID printStreamConstructor = env->GetMethodID(printStreamClass, "<init>", "(Ljava/io/OutputStream;)V");
    if (exceptionRaised(env) || (printStreamConstructor == NULL))
	{
		logError(hEventSource, isStdout ? "Could not find the PrintStream constructor for System.out redirect."
										: "Could not find the PrintStream constructor for System.err redirect.");
		return false;
    }

	//Create a PrintStream.
    jobject printStream = env->NewObject(printStreamClass, printStreamConstructor, fileOutputStream);
    if (exceptionRaised(env) || (printStream == NULL))
	{
		logError(hEventSource, isStdout ? "Could not create a PrintStream for System.out redirect."
										: "Could not create a PrintStream for System.err redirect.");
		return false;
    }

	//Find the System class.
    jclass systemClass = env->FindClass("java/lang/System");
    if (exceptionRaised(env) || (systemClass == NULL))
	{
		logError(hEventSource, isStdout ? "Could not find the System class for System.out redirect."
										: "Could not find the System class for System.err redirect.");
		return false;
    }

	//Find the setOut() method.
    jmethodID setOutMethod = env->GetStaticMethodID(systemClass, "setOut", "(Ljava/io/PrintStream;)V");
    if (exceptionRaised(env) || (setOutMethod == NULL))
	{
		logError(hEventSource, isStdout ? "Could not find the setOut method for System.out redirect."
										: "Could not find the setOut method for System.err redirect.");
		return false;
    }

	//Call System.setOut().
	env->CallStaticVoidMethod(systemClass, setOutMethod, printStream);
	if (exceptionRaised(env))
	{
		logError(hEventSource, isStdout ? "Could not call the setOut method for System.out redirect."
										: "Could not call the setOut method for System.err redirect.");
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
