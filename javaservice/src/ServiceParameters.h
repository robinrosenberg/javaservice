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

#ifndef __SERVICE_PARAMETERS_H__
#define __SERVICE_PARAMETERS_H__


class ServiceParameters
{
public:

	//
	// Default Constructor
	//
	ServiceParameters();

	//
	// Destructor, release any allocated strings or arrays
	//
	~ServiceParameters();

	//
	// Load parameters based on arguments supplied to install command
	//
	bool loadFromArguments(int argc, char* argv[]);
	
	//
	// Load parameters from registry, based on service name
	//
	bool readFromRegistry(const char* serviceName);

	//
	// Initial implementation, all values available with unchecked public access
	//

	// Software version number, for reference only
	const char* swVersion;

	//The location of the jvm library.
	const char *jvmLibrary;

	//The number of jvm options.
	int jvmOptionCount;

	//The jvm options.
	const char **jvmOptions;

	//The start class for the service.
	const char *startClass;

	//The start method for the service.
	const char *startMethod;

	//The number of parameters for the start method.
	int startParamCount;

	//The start method parameters.
	const char **startParams;

	//The stop class for the service.
	const char *stopClass;

	//The stop method for the service.
	const char *stopMethod;

	//The number of parameters for the stop method.
	int stopParamCount;

	//The stop method parameters.
	const char **stopParams;

	//The out redirect file.
	const char *outFile;

	//The err redirect file.
	const char *errFile;

	//The path extension.
	const char *pathExt;

	//The current directory.
	const char *currentDirectory;

	// NT service dependency
	const char* dependsOn;

	// Automatic (default) or manual service startup
	bool autoStart;


	// set this flag if the string values have been allocated and will need to be freed
	void setLoadedDynamically(bool isDynamic) { loadedDynamically = isDynamic; }


private:

	bool loadedDynamically;

	ServiceParameters(const ServiceParameters& other);
	ServiceParameters& operator=(const ServiceParameters& other);
};


// output stream handler, gives formatted output of object contents

class ostream;

ostream& operator<< (ostream& os, const ServiceParameters&);

#endif // __SERVICE_PARAMETERS_H__
