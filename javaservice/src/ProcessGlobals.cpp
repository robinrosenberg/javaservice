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

#include "ProcessGlobals.h"
#include "EventLogger.h"
#include "ServiceParameters.h"
#include "ServiceLogger.h"




//
// [singleton] static process data declarations used in operation of the service
//

ProcessGlobals* ProcessGlobals::globalsInstance = NULL;

//TODO - lock required around creator/accessor of this object instance


// accessor for creation of process global data singleton, checks for duplicate initialisation
ProcessGlobals* ProcessGlobals::createInstance(const char* serviceName, LPHANDLER_FUNCTION serviceHandlerFunction)
{
	if (globalsInstance != NULL)
	{
		ServiceLogger::write("Attempt to create and initialise duplicate instance of ProcessGlobals\n");
		// could also check to see service name is the same (assert), will be the same - or null!
	}
	else
	{
		globalsInstance = new ProcessGlobals();

		if (!globalsInstance->initialise(serviceName, serviceHandlerFunction))
		{
			ServiceLogger::write("Failed to initialise ProcessGlobals singleton instance\n");
			delete globalsInstance;
			globalsInstance = NULL; // application will fail with access violation next...
		}
	}

	return globalsInstance;
}


// general accessor for use of process global data singleton, checks for prior initialisation
ProcessGlobals* ProcessGlobals::getInstance()
{
	if (globalsInstance == NULL)
	{
		ServiceLogger::write("Attempt to use ProcessGlobals before initialisation performed\n");
		// until locking implemented, this could return reference before values in object are set up
	}

	return globalsInstance; // expect application to crash with access violation next...
}


// release any event and event source handles and destroy the singleton

void ProcessGlobals::destroyInstance()
{
	if (globalsInstance == NULL)
	{
		ServiceLogger::write("Invalid attempt to delete uninitialised ProcessGlobals instance (ignored)\n");
	}
	else
	{
		globalsInstance->cleanUp();
		delete globalsInstance;
		globalsInstance = NULL;
	}

}













/*
 * Construct instance of process globals class, with all values used during
 * execution of the background service process.
 */
ProcessGlobals::ProcessGlobals()
: serviceName(NULL)
, serviceParams(NULL)
, hEventSource(NULL)
, hWaitForStart(NULL)
, hWaitForStop(NULL)
, serviceStartedSuccessfully(false)
, serviceStoppedSuccessfully(false)
{
}

/*
 * Set up process globals for the named service, to execute specified handler function
 */
bool ProcessGlobals::initialise(const char* _serviceName, LPHANDLER_FUNCTION serviceHandlerFunction)
{
	// store the supplied service name

	serviceName = _serviceName;

	// set up initial values in service status structure

	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	// create event source (event logger writes to stderr if this is null)

	hEventSource = registerServiceEventSource(serviceName);

	// load service parameters from the registry

	bool initOk = loadServiceParameters();

	if (!initOk)
	{
		logFunctionError(hEventSource, "loadServiceParameters (RegQueryValueEx)");
	}

	// if configured, create the pair of event semaphores to wait on

	if (initOk)
	{
		hWaitForStart = createEventHandle();

		hWaitForStop = createEventHandle();

		initOk = ((hWaitForStart != NULL) && (hWaitForStop != NULL));
	}

	// if ok so far, register the specified control request handler

	if (initOk)
	{
		hServiceStatus = RegisterServiceCtrlHandler(serviceName, serviceHandlerFunction);

		if (hServiceStatus == NULL)
		{
			initOk = false;
			logFunctionError(hEventSource, "RegisterServiceCtrlHandler");
		}
	}

	// if initialisation was not successful, clean up now

	if (!initOk)
	{
		cleanUp();
	}

	return initOk;
}


void ProcessGlobals::cleanUp()
{

	// free any memory taken with service param config

	if (serviceParams != NULL)
	{
		delete serviceParams;
		serviceParams = NULL;
	}

	// release any handles that were obtained
	// (functions will set to null afterwards)

	deregisterEventSource(&hEventSource);
	closeEventHandle(&hWaitForStop);
	closeEventHandle(&hWaitForStart);

	// note that service status handle does not need to be closed
}


ProcessGlobals::~ProcessGlobals()
{
	cleanUp(); // perform any required memory/handle cleanup
}




bool ProcessGlobals::loadServiceParameters()
{
	ServiceParameters* params = ServiceParametersFactory::createFromRegistry(serviceName);

	bool gotParams = (params != NULL);

	if (gotParams)
	{
		serviceParams = params; // save created instance for use globally
	}
	else
	{
		delete params;
		params = NULL;
	}

	return gotParams;
}




void ProcessGlobals::setStatusWaitHint(int waitHint)
{
	serviceStatus.dwWaitHint = waitHint;
}


void ProcessGlobals::updateServiceStatus(int currentState)
{
	// set the specified status value and notify the service manager

	serviceStatus.dwCurrentState = currentState;
	setServiceStatus();
}


void ProcessGlobals::setServiceStatus()
{
	// notify the service manager of current service status
	// (must be called for all service messages, even if status unchanged)

	BOOL sts = SetServiceStatus(hServiceStatus, &serviceStatus);

	if (!sts)
	{
		logFunctionError(hEventSource, "SetServiceStatus");
	}

}


void ProcessGlobals::waitForBothEvents()
{
	HANDLE handles[2] = { hWaitForStart, hWaitForStop };

	DWORD waitStatus = WaitForMultipleObjects(2, handles, TRUE, INFINITE);

	if (waitStatus == WAIT_FAILED)
	{
		logFunctionError(hEventSource, "WaitForMultipleObjects");
	}
}



void ProcessGlobals::setBothEvents()
{
	setStartEvent();
	setStopEvent();
}



void ProcessGlobals::setStartEvent()
{
	if (hWaitForStart != NULL)
	{
		SetEvent(hWaitForStart);
	}
}


void ProcessGlobals::setStopEvent()
{
	if (hWaitForStop != NULL)
	{
		SetEvent(hWaitForStop);
	}
}


void ProcessGlobals::logServiceEvent(int messageType)
{
	logEventMessage(hEventSource, serviceName, messageType);
}


HANDLE ProcessGlobals::createEventHandle()
{
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (hEvent == NULL)
	{
		logFunctionError(hEventSource, "CreateEvent");
	}

	return hEvent;
}


void ProcessGlobals::closeEventHandle(HANDLE* hEvent)
{
	if (*hEvent != NULL)
	{
		BOOL closeOk = CloseHandle(*hEvent);

		if (!closeOk)
		{
			logFunctionError(hEventSource, "CloseHandle");
		}

		*hEvent = NULL;
	}
}

