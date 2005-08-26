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


//
// Local constant definitions
//

// Option to be used when specifying Java class path for JVM invocation
static const char *DEF_CLASS_PATH = "-Djava.class.path=";
static const int DEF_CLASS_PATH_LEN = strlen(DEF_CLASS_PATH);


//
// Local function references
//
static bool loadFromArguments(ServiceParameters& params, int argc, char* argv[]);
static int countOptionalArgs(const char** args, const char* stopAtArg, int maxArgs);
static const char** getOptionalArgs(const char** args, int optionCount);
static int countOptionalArgs(const char** args, const char* stopAtArgs[], int maxArgs);


//
// Load parameters from registry, based on service name
// Returns null pointer if load fails (i.e. service not found)
//
ServiceParameters* ServiceParametersFactory::createFromRegistry(const char* serviceName)
{

	ServiceParameters* params = new ServiceParameters();

	RegistryHandler* registryHandler = new RegistryHandler(serviceName);

	bool populatedOk = registryHandler->readServiceParams(*params);

	delete registryHandler; // don't need this object any more

	if (!populatedOk)
	{
		delete params;
		params = NULL;
	}

	return params;
}



//
// Load parameters based on arguments supplied to install command
// Returns null pointer if load fails (i.e. arguments not valid)
//
ServiceParameters* ServiceParametersFactory::createFromArguments(int argc, char* argv[])
{
	ServiceParameters* params = new ServiceParameters();

	bool populatedOk = loadFromArguments(*params, argc, argv);

	if (!populatedOk)
	{
		delete params;
		params = NULL;
	}

	return params;
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
//		[-append | -overwrite]
//		[-startup seconds]
//
bool loadFromArguments(ServiceParameters& params, int argc, char* argv[])
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

	params.setJvmLibrary(*args++);
	remaining--;

	// following arguments are taken to be JVM options up until '-start' argument

	params.setJvmOptionCount(countOptionalArgs(args, "-start", remaining));
	// note, count set before corresponding array is set up
	if (params.getJvmOptionCount() > 0)
	{
		params.setJvmOptions(getOptionalArgs(args, params.getJvmOptionCount()));
		args += params.getJvmOptionCount();
		remaining -= params.getJvmOptionCount();
	}

	// next two arguments after jvm library and any jvm options must be '-start'
	// and the associated start class name - anything after that is optional

	if ((remaining > 1) && (strcmp(*args, "-start") == 0))
	{
		args++; // skip -start arg
		remaining--;

		params.setStartClass(*args++); // start class name
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

		params.setStartMethod(*args++); // start method name
		remaining--;
	}

	// list of arguments that will end options lists in calls below
	static const char* endingArgs[] = { "-stop", "-out", "-err", "-current", "-path", "-depends", "-auto", "-manual", "-shutdown", "-user", "-password", "-append", "-overwrite", "-startup", NULL };

	// start method parameters

	if (argsOk && (remaining > 1) && (strcmp(*args, "-params") == 0))
	{
		args++; // skip -params arg
		remaining--;

		params.setStartParamCount(countOptionalArgs(args, endingArgs, remaining));

		if (params.getStartParamCount() > 0)
		{
			params.setStartParams(getOptionalArgs(args, params.getStartParamCount()));
			args += params.getStartParamCount();
			remaining -= params.getStartParamCount();
		}
	}

	// optional stop class, method and parameters

	if (argsOk && (remaining > 1) && (strcmp(*args, "-stop") == 0))
	{
		args++; // skip -stop arg
		remaining--;

		params.setStopClass(*args++); // stop class name
		remaining--;

		// optional '-method' and method name arguments present?

		if (argsOk && (remaining > 1) && (strcmp(*args, "-method") == 0))
		{
			args++; // skip -method arg
			remaining--;

			params.setStopMethod(*args++); // stop method name
			remaining--;
		}

		// see if any stop parameters are specified

		if (argsOk && (remaining > 1) && (strcmp(*args, "-params") == 0))
		{
			args++; // skip -params arg
			remaining--;

			// skip first element (-stop) in list of ending arguments in this call
			params.setStopParamCount(countOptionalArgs(args, &endingArgs[1], remaining));

			if (params.getStopParamCount() > 0)
			{
				params.setStopParams(getOptionalArgs(args, params.getStopParamCount()));
				args += params.getStopParamCount();
				remaining -= params.getStopParamCount();
			}
		}
	}

	// optional '-out' filename for stdout

	if (argsOk && (remaining > 1) && (strcmp(*args, "-out") == 0))
	{
		args++; // skip -out arg
		remaining--;

		params.setOutFile(*args++); // stdout filename
		remaining--;
	}

	// optional '-err' filename for stderr

	if (argsOk && (remaining > 1) && (strcmp(*args, "-err") == 0))
	{
		args++; // skip -err arg
		remaining--;

		params.setErrFile(*args++); // stderr filename
		remaining--;
	}

	// optional '-current' directory

	if (argsOk && (remaining > 1) && (strcmp(*args, "-current") == 0))
	{
		args++; // skip -current arg
		remaining--;

		params.setCurrentDirectory(*args++); // current directory location
		remaining--;
	}

	// optional '-path' additional system path definition

	if (argsOk && (remaining > 1) && (strcmp(*args, "-path") == 0))
	{
		args++; // skip -path arg
		remaining--;

		params.setPathExt(*args++); // additional path
		remaining--;
	}

	// optional '-depends' service startup dependency

	if (argsOk && (remaining > 1) && (strcmp(*args, "-depends") == 0))
	{
		args++; // skip -depends arg
		remaining--;

		params.setDependency(*args++); // service dependency
		remaining--;
	}

	// check for optional '-auto' or '-manual' startup control flag

	if (argsOk && (remaining > 0))
	{
		if (strcmp(*args, "-auto") == 0)
		{
			params.setAutoStart(true); // set the flag, although this is defaulted already
			args++;
			remaining--;
		}
		else if (strcmp(*args, "-manual") == 0)
		{
			params.setAutoStart(false);
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
			params.setShutdownMsecs(shutdownSeconds * 1000); // shutdown milliseconds timeout
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

		params.setServiceUser(*args++); // service user (Win2K Active Directory style user@domain)
		remaining--;
	}

	// also look for optional service password (should be defined along with username)

	if (argsOk && (remaining > 1) && (strcmp(*args, "-password") == 0))
	{
		args++; // skip -password arg
		remaining--;

		params.setServicePassword(*args++); // service user password
		remaining--;
	}

	// check that user and password are only ever both specified together

	if (((params.getServiceUser() == NULL) && (params.getServicePassword() != NULL))
	||  ((params.getServiceUser() != NULL) && (params.getServicePassword() == NULL)))
	{
		cerr << "Invalid service parameters specified for install command" << endl;
		cerr << "Service user and password must be specified as a pair, not individually" << endl;
		argsOk = false;
	}

	// check for optional '-append' or '-overwrite' startup control flag

	if (argsOk && (remaining > 0))
	{
		if (strcmp(*args, "-append") == 0)
		{
			params.setFileOverwriteFlag(false); // clear the flag, although this is defaulted already
			args++;
			remaining--;
		}
		else if (strcmp(*args, "-overwrite") == 0)
		{
			params.setFileOverwriteFlag(true); // set the flag, always create new output files
			args++;
			remaining--;
		}
	}

	// see if optional '-startup' service startup delay is specified

	if (argsOk && (remaining > 1) && (strcmp(*args, "-startup") == 0))
	{
		args++; // skip -startup arg
		remaining--;

		// get string value, parse to get number and validate that before using it
		const char* startupString = *args;
		const int startupSeconds = atoi(startupString);

		if (startupSeconds >= 0)
		{
			params.setStartupMsecs(startupSeconds * 1000); // startup milliseconds delay
			args++;
			remaining--;
		}
		else
		{
			cerr << "'-startup seconds' parameter not present on install command" << endl;
			argsOk = false;
		}

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
