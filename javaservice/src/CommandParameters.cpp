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

#include "CommandParameters.h"

/*
 * Class containing set of variables corresponding to command-line parameters,
 * which are a combination of mandatory and optional values according to usage.
 */

//
// Default Constructor
//
CommandParameters::CommandParameters()
: versionEnquiry(false)
, installing(false)
, uninstalling(false)
, serviceName(NULL)
, jvmLibrary(NULL)
, jvmOptionCount(0)
, jvmOptions(NULL)
, startClass(NULL)
, startMethod("main")
, startParamCount(0)
, startParams(NULL)
, stopClass(NULL)
, stopMethod("main") //note, only used if stop class is not null
, stopParamCount(0)
, stopParams(NULL)
, outFile(NULL)
, errFile(NULL)
, pathExt(NULL)
, currentDirectory(NULL)
, dependsOn(NULL)
, autoStart(true)
{
}


//
// Destructor, release any allocated strings or arrays
//
CommandParameters::~CommandParameters()
{
	if (jvmOptions != NULL) delete[] jvmOptions;
	if (startParams != NULL) delete[] startParams;
	if (stopParams != NULL) delete[] stopParams;
}

//
// Populate parameters based on arguments supplied to program (none if running as service)
//
bool CommandParameters::ParseArguments(int argc, char* argv[])
{
	//See if this is an install.
	if (argc >= 2 && strcmp(argv[1], "-install") == 0)
	{
		//Mark that we are installing the service.
		installing = true;

		//Make sure we have the correct number of parameters for installing.
		if (argc >= 6)
		{
			//Use the second parameter as the service name.
			serviceName = argv[2];

			//Use the third parameter as the JVM Library.
			jvmLibrary = argv[3];

			int nextArg = 4;

			//Count the number of jvm options specified.
			for (int i=nextArg; i<argc; i++)
			{
				if (strcmp(argv[i], "-start") == 0)
				{
					break;
				}
				else
				{
					jvmOptionCount++;
				}
			}

			//Get the jvm options, if there are any.
			if (jvmOptionCount > 0)
			{
				jvmOptions = new char*[jvmOptionCount];
				for (i=0; i<jvmOptionCount && nextArg<argc; i++, nextArg++)
				{
					jvmOptions[i] = argv[nextArg];
				}
			}

			//Skip over the next option, which must be -start, if there is one.
			if (nextArg < argc)
			{
				nextArg++;

				//Use the next argument as the class to start.
				if (nextArg < argc)
				{
					startClass = argv[nextArg++];
				}
				else
				{
					return false;
				}
			}

			//See if there is a method for the start class.
			if (nextArg < argc && strcmp(argv[nextArg], "-method") == 0)
			{
				//Skip the -method.
				nextArg++;

				//Use the next argument as the method to start.
				if (nextArg < argc)
				{
					startMethod = argv[nextArg++];
				}
			}

			//See if there are start parameters.
			if (nextArg < argc && strcmp(argv[nextArg], "-params") == 0)
			{
				//Skip the -params.
				nextArg++;

				//Count the number of parameters specified.
				for (i=nextArg; i<argc; i++)
				{
					//NOTE - will not parse correctly if next option is not stop/out/err (there are other options)
					if (strcmp(argv[i], "-stop") == 0 || strcmp(argv[i], "-out") == 0 || strcmp(argv[i], "-err") == 0)
					{
						break;
					}
					else
					{
						startParamCount++;
					}
				}

				//Get the start parameters, if there are any.
				if (startParamCount > 0)
				{
					startParams = new char*[startParamCount];
					for (i=0; i<startParamCount && nextArg<argc; i++, nextArg++)
					{
						startParams[i] = argv[nextArg];
					}
				}
				else
				{
					return false;
				}
			}

			//See if the next option is -stop.
			if (nextArg < argc && strcmp(argv[nextArg], "-stop") == 0)
			{
				//Skip the -stop.
				nextArg++;

				//Use the next argument as the class to stop.
				if (nextArg < argc)
				{
					stopClass = argv[nextArg++];
				}
				else
				{
					return false;
				}

				//See if there is a method for the stop class.
				if (nextArg < argc && strcmp(argv[nextArg], "-method") == 0)
				{
					//Skip the -method.
					nextArg++;

					//Use the next argument as the method to stop.
					if (nextArg < argc)
					{
						stopMethod = argv[nextArg++];
					}
					else
					{
						return false;
					}
				}

				//See if there are stop parameters.
				if (nextArg < argc && strcmp(argv[nextArg], "-params") == 0)
				{
					//Skip the -params.
					nextArg++;

					//Count the number of parameters specified.
					for (i=nextArg; i<argc; i++)
					{
						if (strcmp(argv[i], "-out") == 0 || strcmp(argv[i], "-err") == 0)
						{
							break;
						}
						else
						{
							stopParamCount++;
						}
					}


					//Get the start parameters, if there are any.
					if (stopParamCount > 0)
					{
						stopParams = new char*[stopParamCount];
						for (i=0; i<stopParamCount && nextArg<argc; i++, nextArg++)
						{
							stopParams[i] = argv[nextArg];
						}
					}
					else
					{
						return false;
					}
				}
			}

			//See if there is an out file.
			if (nextArg < argc && strcmp(argv[nextArg], "-out") == 0)
			{
				//Skip the -out.
				nextArg++;

				//Use the next argument as the out file.
				if (nextArg < argc)
				{
					outFile = argv[nextArg++];
				}
				else
				{
					return false;
				}
			}

			//See if there is an err file.
			if (nextArg < argc && strcmp(argv[nextArg], "-err") == 0)
			{
				//Skip the -err.
				nextArg++;

				//Use the next argument as the err file.
				if (nextArg < argc)
				{
					errFile = argv[nextArg++];
				}
				else
				{
					return false;
				}
			}

			//See if there is a current directory.
			if (nextArg < argc && strcmp(argv[nextArg], "-current") == 0)
			{
				//Skip the -current.
				nextArg++;

				//Use the next argument as the extended path
				if (nextArg < argc)
				{
					currentDirectory = argv[nextArg++];
				}
				else
				{
					return false;
				}
			}

			//See if there is any path additions
			if (nextArg < argc && strcmp(argv[nextArg], "-path") == 0)
			{
				//Skip the -path.
				nextArg++;

				//Use the next argument as the extended path
				if (nextArg < argc)
				{
					pathExt = argv[nextArg++];
				}
				else
				{
					return false;
				}
			}

			//See if there is any dependency addition
			if (nextArg < argc && strcmp(argv[nextArg], "-depends") == 0)
			{
				//Skip the -depends.
				nextArg++;

				//Use the next argument as a single service name dependency
				if (nextArg < argc)
				{
					dependsOn = argv[nextArg++];
				}
				else
				{
					return false;
				}
			}

			//See if automatic or manual service startup is specified (defaults to auto mode)
			if (nextArg < argc && strcmp(argv[nextArg], "-auto") == 0)
			{
				//Skip the -auto parameter
				nextArg++;
				autoStart = true; // set the flag, although this is default anyway
			}
			else if (nextArg < argc && strcmp(argv[nextArg], "-manual") == 0)
			{
				//Skip the -manual parameter
				nextArg++;
				autoStart = false; // clear the flag, overriding the default
			}

			//If there are extra parameters, return false.
			if (nextArg < argc)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		return false;
	}

	//See if this is an uninstall.
	else if (argc >= 2 && strcmp(argv[1], "-uninstall") == 0)
	{
		//Mark that we are uninstalling the service.
		uninstalling = true;

		//Make sure we have the correct number of parameters for uninstalling.
		if (argc == 3)
		{
			//Use the second parameter as the service name.
			serviceName = argv[2];

			return true;
		}
		else
		{
			return false;
		}
	}
	
	//See if this is a simple version number enquiry
	else if (argc >= 2 && strcmp(argv[1], "-version") == 0)
	{
		//Mark that we are performing a version number enquiry
		versionEnquiry = true;

		//Make sure we have the correct number of parameters for uninstalling.
		if (argc == 2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//See if the service is starting up (no actual parameters, just local program name)
	else if (argc == 1)
	{
		return true;
	}

	return false;
}
