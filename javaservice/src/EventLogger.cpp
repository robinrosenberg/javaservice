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

#include <iostream.h>
#include "EventLogger.h"
#include "Messages.h"
#include "ServiceLogger.h"




HANDLE registerDefaultEventSource()
{
	return registerServiceEventSource("JavaService"); // default event source for setup messages
}


HANDLE registerServiceEventSource(const char* serviceName)
{

	HANDLE hEventSource = RegisterEventSource(NULL, serviceName);

	if (hEventSource == NULL)
	{
		cerr << "Failed to register " << serviceName << " as event source for logging" << endl << flush;
	}

	return hEventSource; // note, may be null
}


void deregisterEventSource(HANDLE* hEventSource)
{
	if (*hEventSource != NULL)
	{
		DeregisterEventSource(*hEventSource);
		*hEventSource = NULL;
	}
}


void logMessage(HANDLE hEventSource, const char* logText)
{
	logEventMessage(hEventSource, logText, EVENT_GENERIC_INFORMATION);
}


void logError(HANDLE hEventSource, const char* errText)
{
	logEventMessage(hEventSource, errText, EVENT_GENERIC_ERROR);
}


void logEventMessage(HANDLE hEventSource, const char* messageText, int messageType)
{
	
	ServiceLogger::write("logEventMessage: ");
	ServiceLogger::write(messageText);
	ServiceLogger::write("\n");

	if (hEventSource != NULL)
	{
		LPTSTR messages[1] = { (char*)messageText };
		ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, messageType, NULL, 1, 0, (const char **)messages, NULL);
	}
	else
	{
		cerr << "MESSAGE (" << messageType << ") : " << messageText << endl << flush;
	}
}


void logFunctionError(HANDLE hEventSource, const char* functionName)
{
	ServiceLogger::write("logFunctionError: ");
	ServiceLogger::write(functionName);
	ServiceLogger::write("\n");

	LPTSTR messageText = NULL;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&messageText, 0, NULL);

	logFunctionMessage(hEventSource, functionName, messageText);

	LocalFree(messageText);
	
}

void logFunctionMessage(HANDLE hEventSource, const char* functionName, const char* messageText)
{
	ServiceLogger::write("logFunctionMessage: ");
	ServiceLogger::write(messageText);
	ServiceLogger::write("\n");

	LPTSTR messages[2] = { (char*)functionName, (char*)messageText };

	if (hEventSource != NULL)
	{
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_FUNCTION_FAILED, NULL, 2, 0, (const char **)messages, NULL);
	}
	else
	{
		cerr << "ERROR: " << functionName << " : " << messageText << endl << flush;
	}
}
