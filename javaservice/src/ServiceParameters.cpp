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
// Local function references
//
static int countOptionalArgs(const char** args, const char* stopAtArg, int maxArgs);
static const char** getOptionalArgs(const char** args, int optionCount);
static int countOptionalArgs(const char** args, const char* stopAtArgs[], int maxArgs);


//
// Default Constructor
//
ServiceParameters::ServiceParameters()
: swVersion(STRPRODUCTVER)
, jvmLibrary(NULL)
, jvmOptionCount(0)
, jvmOptions(NULL)
, startClass(NULL)
, startMethod("main")
, startParamCount(0)
, startParams(NULL)
, stopClass(NULL)
, stopMethod("main")
, stopParamCount(0)
, stopParams(NULL)
, outFile(NULL)
, errFile(NULL)
, pathExt(NULL)
, currentDirectory(NULL)
, dependsOn(NULL)
, autoStart(true)
, loadedDynamically(false)
{
}


//
// Destructor, release any allocated strings or arrays
//
ServiceParameters::~ServiceParameters()
{
	// allow for two current setup modes - using dynamic strings to be deleted, or input strings to be discarded
	//TODO - supply setProperty functions that always allocate a new string, then always delete them after...
	if (loadedDynamically)
	{
		// some of these strings are not to be deleted, since constants may be supplied instead
		//TODO if (swVersion != NULL) delete[] (void*)swVersion;
		if (jvmLibrary != NULL) delete[] (void*)jvmLibrary;
		if (startClass != NULL) delete[] (void*)startClass;
		//TODO if (startMethod != NULL) delete[] (void*)startMethod;
		if (stopClass != NULL) delete[] (void*)stopClass;
		//TODO if (stopMethod != NULL) delete[] (void*)stopMethod;
		if (outFile != NULL) delete[] (void*)outFile;
		if (errFile != NULL) delete[] (void*)errFile;
		if (pathExt != NULL) delete[] (void*)pathExt;
		if (currentDirectory != NULL) delete[] (void*)currentDirectory;

		if ((jvmOptionCount > 0) && (jvmOptions != NULL))
		{
			for (int i = 0; i < jvmOptionCount; i++)
			{
				delete[] (void*)jvmOptions[i];
			}
		}

		if ((startParamCount > 0) && (startParams != NULL))
		{
			for (int i = 0; i < startParamCount; i++)
			{
				delete[] (void*)startParams[i];
			}
		}

		if ((stopParamCount > 0) && (stopParams != NULL))
		{
			for (int i = 0; i < stopParamCount; i++)
			{
				delete[] (void*)stopParams[i];
			}
		}

	}

	// the arrays of pointers are always dynamic, even if the contents aren't/weren't
	if (jvmOptions != NULL) delete[] jvmOptions;
	if (startParams != NULL) delete[] startParams;
	if (stopParams != NULL) delete[] stopParams;
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

	jvmLibrary = *args++;
	remaining--;

	// following arguments are taken to be JVM options up until '-start' argument

	jvmOptionCount = countOptionalArgs(args, "-start", remaining);

	if (jvmOptionCount > 0)
	{
		jvmOptions = getOptionalArgs(args, jvmOptionCount);
		args += jvmOptionCount;
		remaining -= jvmOptionCount;
	}

	// next two arguments after jvm library and any jvm options must be '-start'
	// and the associated start class name - anything after that is optional

	if ((remaining > 1) && (strcmp(*args, "-start") == 0))
	{
		args++; // skip -start arg
		remaining--;

		startClass = *args++; // start class name
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

		startMethod = *args++; // start method name
		remaining--;
	}

	// list of arguments that will end options lists in calls below
	static const char* endingArgs[] = { "-stop", "-out", "-err", "-current", "-auto", "-manual", NULL };

	// start method parameters

	if (argsOk && (remaining > 1) && (strcmp(*args, "-params") == 0))
	{
		args++; // skip -params arg
		remaining--;

		startParamCount = countOptionalArgs(args, endingArgs, remaining);

		if (startParamCount > 0)
		{
			startParams = getOptionalArgs(args, startParamCount);
			args += startParamCount;
			remaining -= startParamCount;
		}
	}

	// optional stop class, method and parameters

	if (argsOk && (remaining > 1) && (strcmp(*args, "-stop") == 0))
	{
		args++; // skip -stop arg
		remaining--;

		stopClass = *args++; // stop class name
		remaining--;

		// optional '-method' and method name arguments present?

		if (argsOk && (remaining > 1) && (strcmp(*args, "-method") == 0))
		{
			args++; // skip -method arg
			remaining--;

			stopMethod = *args++; // stop method name
			remaining--;
		}

		// see if any stop parameters are specified

		if (argsOk && (remaining > 1) && (strcmp(*args, "-params") == 0))
		{
			args++; // skip -params arg
			remaining--;

			// skip first element (-stop) in list of ending arguments in this call
			stopParamCount = countOptionalArgs(args, &endingArgs[1], remaining);

			if (stopParamCount > 0)
			{
				stopParams = getOptionalArgs(args, stopParamCount);
				args += stopParamCount;
				remaining -= stopParamCount;
			}
		}
	}

	// optional '-out' filename for stdout

	if (argsOk && (remaining > 1) && (strcmp(*args, "-out") == 0))
	{
		args++; // skip -out arg
		remaining--;

		outFile = *args++; // stdout filename
		remaining--;
	}

	// optional '-err' filename for stderr

	if (argsOk && (remaining > 1) && (strcmp(*args, "-err") == 0))
	{
		args++; // skip -err arg
		remaining--;

		errFile = *args++; // stderr filename
		remaining--;
	}

	// optional '-current' directory

	if (argsOk && (remaining > 1) && (strcmp(*args, "-current") == 0))
	{
		args++; // skip -current arg
		remaining--;

		currentDirectory = *args++; // current directory location
		remaining--;
	}

	// optional '-path' additional classpath definition

	if (argsOk && (remaining > 1) && (strcmp(*args, "-path") == 0))
	{
		args++; // skip -path arg
		remaining--;

		pathExt = *args++; // additional path
		remaining--;
	}

	// optional '-depends' service startup dependency

	if (argsOk && (remaining > 1) && (strcmp(*args, "-depends") == 0))
	{
		args++; // skip -depends arg
		remaining--;

		dependsOn = *args++; // service dependency
		remaining--;
	}

	// finally, check for optional '-auto' or '-manual' startup control flag

	if (argsOk && (remaining > 0))
	{
		if (strcmp(*args, "-auto") == 0)
		{
			autoStart = true; // set the flag, although this is defaulted already
			args++;
			remaining--;
		}
		else if (strcmp(*args, "-manual") == 0)
		{
			autoStart = false;
			args++;
			remaining--;
		}
	}

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
		optionalArgs[i] = args[i];
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

	setLoadedDynamically(gotParams); // ensure new strings get released

	return gotParams;
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

	outputConfigString(os, "JavaService Version ", serviceParams.swVersion);

	outputConfigString(os, "JVM Library", serviceParams.jvmLibrary);

	outputConfigArray(os, "JVM Option", serviceParams.jvmOptionCount, serviceParams.jvmOptions);

	outputClassInfo(os, "Start", serviceParams.startClass, serviceParams.startMethod, serviceParams.startParamCount, serviceParams.startParams);

	outputClassInfo(os, "Stop", serviceParams.stopClass, serviceParams.stopMethod, serviceParams.stopParamCount, serviceParams.stopParams);

	outputConfigString(os, "Stdout File", serviceParams.outFile);

	outputConfigString(os, "Stderr File", serviceParams.errFile);

	outputConfigString(os, "Path Extension", serviceParams.pathExt);
	
	outputConfigString(os, "Current Directory", serviceParams.currentDirectory);

	// dependsOn and autostart only used during installation command processing

	os << flush;

	return os;
}

