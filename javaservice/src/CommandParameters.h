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


////
//// Constant Declarations
////

// Number of milliseconds delay timeout when stopping the service
static const long SHUTDOWN_TIMEOUT_MSECS = 30000; // 30 seconds

// Number of milliseconds delay after exit handler has been triggered
static const long EXIT_HANDLER_TIMEOUT_MSECS = 90000; // 90 seconds


/*
 * Class containing set of variables corresponding to command-line parameters,
 * which are a combination of mandatory and optional values according to usage.
 */

class CommandParameters
{
public:

	//
	// Default Constructor
	//
	CommandParameters();

	//
	// Destructor, release any allocated strings or arrays
	//
	~CommandParameters();

	//
	// Populate parameters based on arguments supplied to program (none if running as service)
	//
	bool ParseArguments(int argc, char* argv[]);
	


	//
	// Initial implementation, all values available with unchecked public access
	//


	// Flag for whether we are to simply to display version number information
	bool versionEnquiry;

	//Flag for whether we are trying to install a service.
	bool installing;

	//Flag for whether we are trying to uninstall a service.
	bool uninstalling;

	//The name of the service.
	char *serviceName;

	//The location of the jvm library.
	char *jvmLibrary;

	//The number of jvm options.
	int jvmOptionCount;

	//The jvm options.
	char **jvmOptions;

	//The start class for the service.
	char *startClass;

	//The start method for the service.
	char *startMethod;

	//The number of parameters for the start method.
	int startParamCount;

	//The start method parameters.
	char **startParams;

	//The stop class for the service.
	char *stopClass;

	//The stop method for the service.
	char *stopMethod;

	//The number of parameters for the stop method.
	int stopParamCount;

	//The stop method parameters.
	char **stopParams;

	//The out redirect file.
	char *outFile;

	//The err redirect file.
	char *errFile;

	//The path extension.
	char *pathExt;

	//The current directory.
	char *currentDirectory;

	// NT service dependency
	const char* dependsOn;

	// Automatic (default) or manual service startup
	bool autoStart;

} ;
