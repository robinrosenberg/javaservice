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
	// Data members all have read-only get accessors and copy-type set modifiers
	//

	void setSwVersion(const char* wotVersion) { updateStringValue(swVersion, wotVersion); }
	const char* getSwVersion() const { return swVersion; }

	void setJvmLibrary(const char* wotLibrary) { updateStringValue(jvmLibrary, wotLibrary); }
	const char* getJvmLibrary() const { return jvmLibrary; }

	void setJvmOptionCount(int wotCount);
	int getJvmOptionCount() const { return jvmOptionCount; }
	void setJvmOption(int optionIndex, const char* wotOption) { updateStringValue(jvmOptions[optionIndex], wotOption); }
	const char* getJvmOption(int optionIndex) const { return jvmOptions[optionIndex]; }
	void setJvmOptions(const char** wotOptions);
	const char** getJvmOptions() const { return jvmOptions; }

	void setStartClass(const char* wotClass) { updateStringValue(startClass, wotClass); }
	const char* getStartClass() const { return startClass; }

	void setStartMethod(const char* wotMethod) { updateStringValue(startMethod, wotMethod); }
	const char* getStartMethod() const { return startMethod; }

	void setStartParamCount(int wotCount);
	int getStartParamCount() const { return startParamCount; }
	void setStartParam(int paramIndex, const char* wotParam) { updateStringValue(startParams[paramIndex], wotParam); }
	const char* getStartParam(int paramIndex) const { return startParams[paramIndex]; }
	void setStartParams(const char** wotParams);
	const char** getStartParams() const { return startParams; }

	void setStopClass(const char* wotClass) { updateStringValue(stopClass, wotClass); }
	const char* getStopClass() const { return stopClass; }

	void setStopMethod(const char* wotMethod) { updateStringValue(stopMethod, wotMethod); }
	const char* getStopMethod() const { return stopMethod; }


	void setStopParamCount(int wotCount);
	int getStopParamCount() const { return stopParamCount; }
	void setStopParams(const char** wotParams);
	const char** getStopParams() const { return stopParams; }
	void setStopParam(int paramIndex, const char* wotParam) { updateStringValue(stopParams[paramIndex], wotParam); }
	const char* getStopParam(int paramIndex) const { return stopParams[paramIndex]; }

	void setOutFile(const char* wotFile) { updateStringValue(outFile, wotFile); }
	const char* getOutFile() const { return outFile; }

	void setErrFile(const char* wotFile) { updateStringValue(errFile, wotFile); }
	const char* getErrFile() const { return errFile; }

	void setPathExt(const char* wotPathExtension) { updateStringValue(pathExt, wotPathExtension); }
	const char* getPathExt() const { return pathExt; }

	void setCurrentDirectory(const char* wotDirectory) { updateStringValue(currentDirectory, wotDirectory); }
	const char* getCurrentDirectory() const { return currentDirectory; }

	void setDependency(const char* wotDependency) { updateStringValue(dependency, wotDependency); }
	const char* getDependency() const { return dependency; }

	void setAutoStart(bool isAutoStart) { autoStart = isAutoStart; }
	bool isAutoStart() const { return autoStart; }

	void setShutdownMsecs(int wotMsecs) { shutdownMsecs = wotMsecs; }
	int getShutdownMsecs() const { return shutdownMsecs; }

private:

	const char* swVersion;			// Software version number, for reference only

	const char *jvmLibrary;			// the jvm library dll of the run-time environment
	int jvmOptionCount;				// number of jvm options
	const char **jvmOptions;		// the jvm option(s)


	const char *startClass;			// start class for the service
	const char *startMethod;		// start method for the service
	int startParamCount;			// number of parameters for the start method
	const char **startParams;		// start method parameters

	const char *stopClass;			// stop class for the service
	const char *stopMethod;			// stop method for the service
	int stopParamCount;				// number of parameters for the stop method
	const char **stopParams;		// stop method parameters

	const char *outFile;			// java stdout redirect file
	const char *errFile;			// java stderr redirect file
	const char *pathExt;			// path extension
	const char *currentDirectory;	// run-time directory
	const char* dependency;			// service dependency
	bool autoStart;					// Automatic (default) or manual service startup

	int shutdownMsecs;				// milliseconds shutdown timeout

	void updateStringValue(const char*& stringRef, const char* newString);


	// private access copy functions, not implemented / not used
	ServiceParameters(const ServiceParameters& other);
	ServiceParameters& operator=(const ServiceParameters& other);
};


// output stream handler, gives formatted output of object contents

class ostream;

ostream& operator<< (ostream& os, const ServiceParameters&);

#endif // __SERVICE_PARAMETERS_H__
