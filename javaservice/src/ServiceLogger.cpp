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

#include "ServiceLogger.h"
//TODO #include <iostream.h>
#include <stdio.h>
#include <string.h>


//
// Local constants
//

// default mode for logging enabled/disabled depends on build mode used
#if defined(_DEBUG)
#define LOGGING_ENABLED_DEFAULT true
#else
#define LOGGING_ENABLED_DEFAULT false
#endif

// hard-coded filename to be used for logging if not specified before use
//TODO - may try using %TEMP% or %SYSTEMROOT% environment variables in the name
#define DEFAULT_LOGFILE_NAME "c:\\javaservice.log"




//
// Static class data
//

// flag controlling whether or not service logging is in use (true in debug build)
bool ServiceLogger::loggingEnabled = LOGGING_ENABLED_DEFAULT;

// last-specified log output filename, if any
const char* ServiceLogger::logOutput = NULL;

// singleton instance, created on first use (if logging enabled)
ServiceLogger* ServiceLogger::instance = NULL;


//
// Static class functions, use singleton to perform requested operation
//

// output text string to the logger output file/device
void ServiceLogger::write(const char* message)
{
	getInstance().writeLog(message);
}


// specifically close the output log file
void ServiceLogger::close()
{
	getInstance().closeLogFile();
}


// singleton accessor for the logger class, creates instance if not yet instantiated
ServiceLogger& ServiceLogger::getInstance()
{
	// note that this does not include any thread-safe synchronisation
	if (instance == NULL)
	{
		instance = new ServiceLogger();
	}

	return *instance;
}


// delete singleton instance, due to change in logging configuration
void ServiceLogger::deleteInstance()
{
	// note that this does not include any thread-safe synchronisation
	if (instance != NULL)
	{
		instance->closeLogFile();
		delete instance;
		instance = NULL;
	}

}


// set control flag true to enable logging to specified file
void ServiceLogger::enableLogging(const char* outputFile)
{
	// check to see if logging instance already created with different settings

	if (instance != NULL)
	{
		if (!loggingEnabled ||
			((outputFile == NULL) && (logOutput != NULL)) ||
			((outputFile != NULL) && (logOutput == NULL)) ||
			((outputFile != NULL) && !strcmp(outputFile, logOutput)))
		{
			deleteInstance();
		}
	}

	loggingEnabled = true;
	logOutput = outputFile;
}


// set control flag false to disable logging
void ServiceLogger::disableLogging()
{
	if ((instance != NULL) && loggingEnabled)
	{
		deleteInstance();
	}
	loggingEnabled = false;
}


//
// Logging class implementation
//

// constructor, checks whether logging to be enabled or not, opens output file
ServiceLogger::ServiceLogger()
: logFile(NULL)
{
	if (loggingEnabled)
	{
		logFile = fopen(logOutput, "a+");
		writeLog("\n***** Start of new JavaService log session *****\n");
	}
}

// write supplied text to the output log file
void ServiceLogger::writeLog(const char* message)
{
	if (loggingEnabled && (logFile != NULL))
	{
		fprintf(logFile, message);
		fflush(logFile);
	}
}

// close log file, if open, after writing end-of-session text
void ServiceLogger::closeLogFile()
{
	if (loggingEnabled && (logFile != NULL))
	{
		writeLog("\n***** End of JavaService log session *****\n");
		fflush(logFile);
		fclose(logFile);
		logFile = NULL;
	}
}


// destructor, closes any open log file
ServiceLogger::~ServiceLogger()
{
	closeLogFile();
}
