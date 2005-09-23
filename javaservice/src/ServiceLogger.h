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

#ifndef __SERVICE_LOGGER_H__
#define __SERVICE_LOGGER_H__

#include <stdio.h>

//
// Debug-type output logging to file, for use when debugging background service operations
//
class ServiceLogger
{

public:

	// output text string to the logger output file/device
	static void write(const char* message);

	// output number as string to the logger output file/device
	static void write(const int number);

	// specifically close the output log file
	static void close();

	// singleton accessor for the logger class
	static ServiceLogger& getInstance();

	// set control flag true to enable logging to specified file
	static void enableLogging(const char* logOutput);

	// set control flag false to disable logging
	static void disableLogging();

private:

	// flag controlling whether or not service logging is in use (true in debug build)
	static bool loggingEnabled;

	// last-specified log output filename, if any
	static const char* logOutput;

	// singleton instance, created on first use (if logging enabled)
	static ServiceLogger* instance;

	// delete singleton instance, due to change in logging configuration
	static void deleteInstance();


	// the output file being used for logging, may be set to stdout
	FILE* logFile;


	// constructor, checks whether logging to be enabled or not, opens output file
	ServiceLogger();

	// write supplied text to the output log file
	void writeLog(const char* message);

	// close log file, if open, after writing end-of-session text
	void closeLogFile();

	// destructor, closes any open log file
	~ServiceLogger();


	// copy constructor and operators private and not implemented
	ServiceLogger(const ServiceLogger& other);
	ServiceLogger& operator=(const ServiceLogger& other);
};

#endif // __SERVICE_LOGGER_H__
