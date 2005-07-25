/*
 * JavaService - Windows NT Service Daemon for Java applications
 *
 * Copyright (C) 2005 Multiplan Consultants Ltd.
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
 * This software is derived from earlier work by Alexandria Software Consulting,
 * (no longer contactable) which was released under a BSD-style license in 2001.
 * The V2.x software is a new development issued under the LGPL license alone.
 *
 */

#include <windows.h>
#include <iostream.h>
#include "ServiceParameters.h"
#include "RegistryHandler.h"
#include "VersionNo.h"


//
// Local constant definitions
//

// Number of milliseconds delay timeout when stopping the service, default value
static const long DEFAULT_SHUTDOWN_TIMEOUT_MSECS = 30000; // 30 seconds

// Option to be used when specifying Java class path for JVM invocation
static const char *DEF_CLASS_PATH = "-Djava.class.path=";
static const int DEF_CLASS_PATH_LEN = strlen(DEF_CLASS_PATH);


//
// Local function references
//
static void deleteStringArray(int& count, const char**& array);
static int countOptionalArgs(const char** args, const char* stopAtArg, int maxArgs);
static const char** getOptionalArgs(const char** args, int optionCount);
static int countOptionalArgs(const char** args, const char* stopAtArgs[], int maxArgs);


//
// Default Constructor
//
ServiceParameters::ServiceParameters()
: swVersion(NULL)
, jvmLibrary(NULL)
, jvmOptionCount(0)
, jvmOptions(NULL)
, startClass(NULL)
, startMethod(NULL)
, startParamCount(0)
, startParams(NULL)
, stopClass(NULL)
, stopMethod(NULL)
, stopParamCount(0)
, stopParams(NULL)
, outFile(NULL)
, errFile(NULL)
, pathExt(NULL)
, currentDirectory(NULL)
, dependency(NULL)
, autoStart(true)
, shutdownMsecs(DEFAULT_SHUTDOWN_TIMEOUT_MSECS)
, serviceUser(NULL)
, servicePassword(NULL)
{
	setSwVersion(STRPRODUCTVER);
	setStartMethod("main");
	setStopMethod("main");
}


//
// Destructor, release any allocated strings or arrays
//
ServiceParameters::~ServiceParameters()
{
	if (swVersion != NULL) delete[] (void*)swVersion;
	if (jvmLibrary != NULL) delete[] (void*)jvmLibrary;
	if (startClass != NULL) delete[] (void*)startClass;
	if (startMethod != NULL) delete[] (void*)startMethod;
	if (stopClass != NULL) delete[] (void*)stopClass;
	if (stopMethod != NULL) delete[] (void*)stopMethod;
	if (outFile != NULL) delete[] (void*)outFile;
	if (errFile != NULL) delete[] (void*)errFile;
	if (pathExt != NULL) delete[] (void*)pathExt;
	if (currentDirectory != NULL) delete[] (void*)currentDirectory;
	if (serviceUser != NULL) delete[] (void*)serviceUser;
	if (servicePassword != NULL) delete[] (void*)servicePassword;

	deleteStringArray(jvmOptionCount, jvmOptions);
	deleteStringArray(startParamCount, startParams);
	deleteStringArray(stopParamCount, stopParams);
}

static void deleteStringArray(int& count, const char**& array)
{
	if ((count > 0) && (array != NULL))
	{
		for (int i = 0; i < count; i++)
		{
			const char* ptr = array[i];
			if (ptr != NULL)
			{
				delete[] (void*)ptr;
				array[i] = NULL;
			}
		}
		delete[] array;
		array = NULL;
		//DONT DO THIS HERE [YET] count = 0;
	}

}

//
// Load parameters based on arguments supplied to install command.
//
// Note that the argc/argv parameters passed to this function should be set up so that
// the count is of arguments *after* the service name, so argv[0] is the JVM library
// and minimum number of arguments is three (jvmLibrary, -start, startClass)
//
// Return true if arguments appear to be valid and correct, false if invalid
//
// Rules are positional and require parameters in correct order
// Command format is:
// 'JavaService'
//		-install serviceName
//		jvmLibrary [jvmOptions...]
//		-start startClass
//			[-method startMethod]
//			[-params stopParams...]
//		[-stop stopClass]
//			[-method stopMethod]
//			[-params stopParams]
//		[-out outFile]
//		[-err errFile]
//		[-current currentDir]
//		[-path extraPath
//		[-depends serviceDependency]
//		[-auto | -manual]
//		[-shutdown seconds]
//		[-user user@domain -password password]
//
bool ServiceParameters::loadFromArguments(int argc, char* argv[])
{

	// perform initial check for minimum parameters first (fail fast)
	if (argc < 3)
	{
		cerr << "Insufficient parameters to install service (jvm -start class)" << endl;
		return false;
	}

	// pointer moved on and counter decremented as arguments are used up
	const char** args = (const char**)argv;
	int remaining = argc;
	bool argsOk = true;

	// first argument (mandatory) is the JVM Library

	setJvmLibrary(*args++);
	remaining--;

	// following arguments are taken to be JVM options up until '-start' argument

	setJvmOptionCount(countOptionalArgs(args, "-start", remaining));
	// note, count set before corresponding array is set up
	if (getJvmOptionCount() > 0)
	{
		setJvmOptions(getOptionalArgs(args, getJvmOptionCount()));
		args += getJvmOptionCount();
		remaining -= getJvmOptionCount();
	}

	// next two arguments after jvm library and any jvm options must be '-start'
	// and the associated start class name - anything after that is optional

	if ((remaining > 1) && (strcmp(*args, "-start") == 0))
	{
		args++; // skip -start arg
		remaining--;

		setStartClass(*args++); // start class name
		remaining--;
	}
	else // start argument(s) not present after JVM parameter(s)
	{
		cerr << "Mandatory '-start class' parameters not present to install service" << endl;
		argsOk = false;
	}

	// start method name

	if (argsOk && (remaining > 1) && (strcmp(*args, "-method") == 0))
	{
		args++; // skip -method arg
		remaining--;

		setStartMethod(*args++); // start method name
		remaining--;
	}

	// list of arguments that will end options lists in calls below
	//static const char* endingArgs[] = { "-stop", "-out", "-err", "-current", "-auto", "-manual", "-shutdown", "-user", "-password", NULL };
	static const char* endingArgs[] = { "-stop", "-out", "-err", "-current", "-path", "-depends", "-auto", "-manual", "-shutdown", "-user", "-password", NULL };

	// start method parameters

	if (argsOk && (remaining > 1) && (strcmp(*args, "-params") == 0))
	{
		args++; // skip -params arg
		remaining--;

		setStartParamCount(countOptionalArgs(args, endingArgs, remaining));

		if (getStartParamCount() > 0)
		{
			setStartParams(getOptionalArgs(args, getStartParamCount()));
			args += getStartParamCount();
			remaining -= getStartParamCount();
		}
	}

	// optional stop class, method and parameters

	if (argsOk && (remaining > 1) && (strcmp(*args, "-stop") == 0))
	{
		args++; // skip -stop arg
		remaining--;

		setStopClass(*args++); // stop class name
		remaining--;

		// optional '-method' and method name arguments present?

		if (argsOk && (remaining > 1) && (strcmp(*args, "-method") == 0))
		{
			args++; // skip -method arg
			remaining--;

			setStopMethod(*args++); // stop method name
			remaining--;
		}

		// see if any stop parameters are specified

		if (argsOk && (remaining > 1) && (strcmp(*args, "-params") == 0))
		{
			args++; // skip -params arg
			remaining--;

			// skip first element (-stop) in list of ending arguments in this call
			setStopParamCount(countOptionalArgs(args, &endingArgs[1], remaining));

			if (getStopParamCount() > 0)
			{
				setStopParams(getOptionalArgs(args, getStopParamCount()));
				args += getStopParamCount();
				remaining -= getStopParamCount();
			}
		}
	}

	// optional '-out' filename for stdout

	if (argsOk && (remaining > 1) && (strcmp(*args, "-out") == 0))
	{
		args++; // skip -out arg
		remaining--;

		setOutFile(*args++); // stdout filename
		remaining--;
	}

	// optional '-err' filename for stderr

	if (argsOk && (remaining > 1) && (strcmp(*args, "-err") == 0))
	{
		args++; // skip -err arg
		remaining--;

		setErrFile(*args++); // stderr filename
		remaining--;
	}

	// optional '-current' directory

	if (argsOk && (remaining > 1) && (strcmp(*args, "-current") == 0))
	{
		args++; // skip -current arg
		remaining--;

		setCurrentDirectory(*args++); // current directory location
		remaining--;
	}

	// optional '-path' additional system path definition

	if (argsOk && (remaining > 1) && (strcmp(*args, "-path") == 0))
	{
		args++; // skip -path arg
		remaining--;

		setPathExt(*args++); // additional path
		remaining--;
	}

	// optional '-depends' service startup dependency

	if (argsOk && (remaining > 1) && (strcmp(*args, "-depends") == 0))
	{
		args++; // skip -depends arg
		remaining--;

		setDependency(*args++); // service dependency
		remaining--;
	}

	// check for optional '-auto' or '-manual' startup control flag

	if (argsOk && (remaining > 0))
	{
		if (strcmp(*args, "-auto") == 0)
		{
			setAutoStart(true); // set the flag, although this is defaulted already
			args++;
			remaining--;
		}
		else if (strcmp(*args, "-manual") == 0)
		{
			setAutoStart(false);
			args++;
			remaining--;
		}
	}

	// see if optional '-shutdown' service stop timeout is specified

	if (argsOk && (remaining > 1) && (strcmp(*args, "-shutdown") == 0))
	{
		args++; // skip -shutdown arg
		remaining--;

		// get string value, parse to get number and validate that before using it
		const char* shutdownString = *args;
		const int shutdownSeconds = atoi(shutdownString);

		if (shutdownSeconds >= 0)
		{
			setShutdownMsecs(shutdownSeconds * 1000); // shutdown milliseconds timeout
			args++;
			remaining--;
		}
		else
		{
			cerr << "'-shutdown seconds' parameter not present on install command" << endl;
			argsOk = false;
		}

	}

	// look for optional service user (user\domain, .\domain or user@domain)

	if (argsOk && (remaining > 1) && (strcmp(*args, "-user") == 0))
	{
		args++; // skip -user arg
		remaining--;

		setServiceUser(*args++); // service user (Win2K Active Directory style user@domain)
		remaining--;
	}

	// also look for optional service password (should be defined along with username)

	if (argsOk && (remaining > 1) && (strcmp(*args, "-password") == 0))
	{
		args++; // skip -password arg
		remaining--;

		setServicePassword(*args++); // service user password
		remaining--;
	}

	// check that user and password are only ever both specified together

	if (((getServiceUser() == NULL) && (getServicePassword() != NULL))
	||  ((getServiceUser() != NULL) && (getServicePassword() == NULL)))
	{
		cerr << "Invalid service parameters specified for install command" << endl;
		cerr << "Service user and password must be specified as a pair, not individually" << endl;
		argsOk = false;
	}

	// verify that there are no remaining, unrecognised parameters on the command

	if (argsOk && (remaining > 0))
	{
		cerr << "Unrecognised or incorrectly-ordered parameters for install command" << endl;
		cerr << "The last " << remaining << " parameters (from '" << *args << "') were not recognised" << endl;
		argsOk = false;
	}

	return argsOk; // true indicates parameters set up/default ok, false if args invalid
}


//
// count number of arguments from array to either the stop value, or end of list
//
static int countOptionalArgs(const char** args, const char* stopAtArg, int maxArgs)
{
	int optionCount = 0;
	bool foundStopArg = false;

	for (int i = 0; (i < maxArgs) && !foundStopArg; i++)
	{
		if (strcmp(args[i], stopAtArg) == 0)
		{
			foundStopArg = true;
		}
		else
		{
			optionCount++;
		}
	}

	return optionCount;

}


//
// count number of arguments from array to any of the stop values, or end of list
//
static int countOptionalArgs(const char** args, const char* stopAtArgs[], int maxArgs)
{
	int optionCount = 0;
	bool foundStopArg = false;

	for (int i = 0; (i < maxArgs) && !foundStopArg; i++)
	{
		for (int j = 0; (stopAtArgs[j] != NULL) && !foundStopArg; j++)
		{
			if (strcmp(args[i], stopAtArgs[j]) == 0)
			{
				foundStopArg = true;
			}
		}

		if (!foundStopArg)
		{
			optionCount++;
		}
	}

	return optionCount;

}



static const char** getOptionalArgs(const char** args, int optionCount)
{

	const char** optionalArgs = new const char*[optionCount + 1];

	for (int i = 0; i < optionCount; i++)
	{
		int arglen = strlen(args[i]);
		char* arg = new char[arglen + 1];
		strcpy(arg, args[i]);
		arg[arglen] = '\0';
		optionalArgs[i] = arg;
	}

	optionalArgs[optionCount] = NULL; // end list with null entry

	return optionalArgs;
}



//
// Load parameters from registry, based on service name
//
bool ServiceParameters::readFromRegistry(const char* serviceName)
{

	RegistryHandler* registryHandler = new RegistryHandler(serviceName);

	bool gotParams = registryHandler->readServiceParams(*this);

	delete registryHandler; // don't need this object any more

	return gotParams;
}



void ServiceParameters::setJvmOptionCount(int wotCount)
{
	if (jvmOptionCount != wotCount) {

		deleteStringArray(jvmOptionCount, jvmOptions);

		jvmOptionCount = wotCount;

		if (jvmOptionCount > 0)
		{
			jvmOptions = (const char**) new char*[jvmOptionCount];
			for (int i = 0; i < jvmOptionCount; i++)
			{
				jvmOptions[i] = NULL;
			}
		}
	}
}


void ServiceParameters::setJvmOptions(const char** wotOptions)
{

	// special case handling included for class path definitions

    // loop through the options array and find out if java.class.path is set by the caller
	// NOTE - could be specified using three different mechanisms, check for all of them

    // accept -cp=xxx or -classpath=xxx as well as -Djava.class.path=xxx
	// if either of these first two forms exist, convert to the latter

	bool classPathOptionIsSet = false;
	const int optionCount = getJvmOptionCount(); // note, set before call to setJvmOptions (yuk)

    for (int i=0; (i < optionCount) && !classPathOptionIsSet; i++)
    {
        if (strstr(wotOptions[i], DEF_CLASS_PATH) != NULL)
		{
			// if this option explicitly specified, then do nothing extra for classpath
            classPathOptionIsSet = true;
        }
        else if ((strstr(wotOptions[i], "-classpath") != NULL)
			 ||  (strstr(wotOptions[i], "-cp") != NULL))
		{
			// these options are not valid for JVM invocation, so if present then replace
			// option string with the correct environment variable definition option instead

			const char* originalOption = wotOptions[i];
			const char* equalsPos = strstr(originalOption, "=");
			if (equalsPos != NULL)
			{
				const char* originalValue = equalsPos + 1;
				const int newOptionLen = DEF_CLASS_PATH_LEN + strlen(originalValue) + 1;

		        char *newOption = (char *)malloc(newOptionLen);
		        strcpy(newOption, DEF_CLASS_PATH);
		        strcat(newOption, originalValue);

				wotOptions[i] = newOption; // note, old string is not released (minor memory leak)
	            classPathOptionIsSet = true;
			}
		}
    }

	// due to command line length issues, CLASSPATH env variable may be used
	// if class path has not been explicitly included in the options list

	if (!classPathOptionIsSet)
	{
		// check for local 'CLASSPATH' definition to be used by the service
	    const char *classPathEnvVar = getenv("CLASSPATH");
		const bool classPathEnvVarIsSet = (classPathEnvVar != NULL);

		if (classPathEnvVarIsSet)
		{
			// add -Djava.class.path=envDefinition to list of jvm options

			const int newOptionLen = DEF_CLASS_PATH_LEN + strlen(classPathEnvVar) + 1;

	        char *newOption = (char *)malloc(newOptionLen);
	        strcpy(newOption, DEF_CLASS_PATH);
	        strcat(newOption, classPathEnvVar);

			// create larger string array, copy elements, add new one

			const char** extendedOptions = new const char*[optionCount + 1];

			for (int i = 0; i < optionCount; i++)
			{
				extendedOptions[i] = wotOptions[i];
			}

			extendedOptions[optionCount] = (const char*) newOption;

			setJvmOptionCount(optionCount + 1); // increment arg count to suit
		}
	}


	// clean up any existing option array, store supplied list of options instead
	deleteStringArray(jvmOptionCount, jvmOptions);
	jvmOptions = wotOptions;
}

void ServiceParameters::setStartParams(const char** wotParams)
{
	deleteStringArray(startParamCount, startParams);

	startParams = wotParams;
}

void ServiceParameters::setStopParams(const char** wotParams)
{
	deleteStringArray(stopParamCount, stopParams);

	stopParams = wotParams;
}

void ServiceParameters::setStartParamCount(int wotCount)
{
	if (startParamCount != wotCount) {

		deleteStringArray(startParamCount, startParams);

		startParamCount = wotCount;

		if (startParamCount > 0)
		{
			startParams = (const char**) new char*[startParamCount];
			for (int i = 0; i < startParamCount; i++)
			{
				startParams[i] = NULL;
			}
		}
	}
}

void ServiceParameters::setStopParamCount(int wotCount)
{
	if (stopParamCount != wotCount) {

		deleteStringArray(stopParamCount, stopParams);

		stopParamCount = wotCount;

		if (stopParamCount > 0)
		{
			stopParams = (const char**) new char*[stopParamCount];
			for (int i = 0; i < stopParamCount; i++)
			{
				stopParams[i] = NULL;
			}
		}
	}
}

void ServiceParameters::updateStringValue(const char*& stringRef, const char* newString)
{
	if (stringRef != NULL)
	{
		delete[] (void*)stringRef;
		stringRef = NULL;
	}

	if (newString != NULL)
	{
		int stringLen = strlen(newString);
		char* allocated = new char[stringLen + 1];
		strcpy(allocated, newString);
		allocated[stringLen] = '\0';

		stringRef = allocated;
	}
}


static bool outputConfigString(ostream& os, const char* stringType, const char* stringEntry)
{

	if (stringEntry == NULL)
	{
		os << stringType << " UNDEFINED" << endl;
		return false;
	}
	else
	{
		os << stringType << "\t" << stringEntry << endl;
		return true;
	}
}


static bool outputConfigValue(ostream& os, const char* intType, const int intEntry)
{

	os << intType << "\t" << intEntry << endl;
	return true;
}


static void outputConfigArray(ostream& os, const char* arrayType, int arraySize, const char** arrayEntries)
{

	os << arrayType << " Count\t" << arraySize << endl;

	for (int i = 0; i < arraySize; i++)
	{
		os << arrayType << " #" << (i + 1) << "\t" << arrayEntries[i] << endl;
	}
}


static void outputClassInfo(ostream& os, const char* classType, const char* className, const char* methodName, int paramCount, const char** params)
{

	if (className == NULL)
	{
		os << classType << " Class UNDEFINED" << endl;
	}
	else
	{
		os << classType << " Class\t" << className << endl;
		os << classType << " Method\t" << methodName << endl;

		os << classType << " Parameter Count\t" << paramCount << endl;
		for (int i = 0; i < paramCount; i++)
		{
			os << classType << " Parameter #" << (i + 1) << "\t" << params[i] << endl;
		}
	}
}


ostream& operator<< (ostream& os, const ServiceParameters& serviceParams)
{

	os << "Service configuration values:-" << endl;

	outputConfigString(os, "JavaService Version ", serviceParams.getSwVersion());

	outputConfigString(os, "JVM Library", serviceParams.getJvmLibrary());

	outputConfigArray(os, "JVM Option", serviceParams.getJvmOptionCount(), serviceParams.getJvmOptions());

	outputClassInfo(os, "Start", serviceParams.getStartClass(), serviceParams.getStartMethod(), serviceParams.getStartParamCount(), serviceParams.getStartParams());

	outputClassInfo(os, "Stop", serviceParams.getStopClass(), serviceParams.getStopMethod(), serviceParams.getStopParamCount(), serviceParams.getStopParams());

	outputConfigString(os, "Stdout File", serviceParams.getOutFile());

	outputConfigString(os, "Stderr File", serviceParams.getErrFile());

	outputConfigString(os, "Path Extension", serviceParams.getPathExt());

	outputConfigString(os, "Current Directory", serviceParams.getCurrentDirectory());

	outputConfigValue(os, "Shutdown Timeout", serviceParams.getShutdownMsecs());

	outputConfigString(os, "Service User", serviceParams.getServiceUser());

	outputConfigString(os, "Service Password", serviceParams.getServicePassword());

	// dependsOn and autostart only used during installation command processing

	os << flush;

	return os;
}

