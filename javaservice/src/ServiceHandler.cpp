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

#include "ServiceHandler.h"
#include <windows.h>
#include <stdio.h>
#include <iostream.h>
#include "Messages.h"
#include "JavaInterface.h"
#include "ServiceParameters.h"
#include "ProcessGlobals.h"
#include "ServiceLogger.h"
#include "EventLogger.h"


////
//// Constant Declarations
////

// Number of milliseconds delay timeout when stopping the service
static const long SHUTDOWN_TIMEOUT_MSECS = 30000; // 30 seconds

// Number of milliseconds for shutdown status update interval
static const long SERVICE_SHUTDOWN_MSECS = SHUTDOWN_TIMEOUT_MSECS + 5000;

// Number of milliseconds delay after exit handler has been triggered
static const long EXIT_HANDLER_TIMEOUT_MSECS = 90000; // 90 seconds



////
//// Local function prototypes
////

static bool setExtendedPath(const char* pathExt);
static HANDLE createThread(LPTHREAD_START_ROUTINE threadFunction, const char* threadName);


////
//// Global function prototypes
////

static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
static void WINAPI ServiceControlHandler(DWORD opcode);
static DWORD WINAPI StartServiceThread(LPVOID lpParam);
static DWORD WINAPI StopServiceThread(LPVOID lpParam);
static DWORD WINAPI TimeoutStopThread(LPVOID lpParam);



bool ServiceHandler::invokeWindowsService()
{

	// Service table lists service name and main program entry point supported here

	static SERVICE_TABLE_ENTRY const serviceTable[] =
	{
		{"JavaService", ServiceMain}, // note service name is ignored here for type _OWN_PROCESS
		{NULL, NULL}
	};



	ServiceLogger::write("Invoking Windows Service, register service control dispatcher\n");

	// Start the service dispatcher, which will call the ServiceMain function for the service
	// (or do nothing at all if program invoked from command line with no parameters)

	BOOL startSuccess = StartServiceCtrlDispatcher(serviceTable);

	// note that it does not return from this call until service is terminated...

	// if this failed, log an event (uses default JavaService event source, as service name not known)
	if (!startSuccess)
	{
		ServiceLogger::write("Service control dispatcher registration failed!\n");

		HANDLE hDefaultEventSource = registerDefaultEventSource();
		logFunctionError(hDefaultEventSource, "StartServiceCtrlDispatcher");
		deregisterEventSource(&hDefaultEventSource);
	}

	ServiceLogger::close();

	return startSuccess ? true : false;
}











//
// static process data declarations used in operation of the service
//

static ProcessGlobals* processGlobals = NULL;




static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	const char* const serviceName = lpszArgv[0];

	ServiceLogger::write("ServiceMain invoked (");
	ServiceLogger::write(serviceName);
	ServiceLogger::write(")\n");

	cout << "ServiceMain invoked, with " << dwArgc << " parameters (service " << serviceName << ")" << endl;

	// service name provided as first argument when background process is run
	// so supply that and the control handler function for initialisation

	processGlobals = new ProcessGlobals();

	if (!processGlobals->initialise(serviceName, ServiceControlHandler))
	{
		ServiceLogger::write("Failed to initialise ProcessGlobals\n");
		delete processGlobals;
		return; // give up now, failed to set up required data
	}


	ServiceLogger::write("Logging service event start[ed] event...\n");
	//Log that we have started. (TODO, or are starting - jvm may fail, thread may fail, class start may fail)

	processGlobals->logServiceEvent(EVENT_SERVICE_STARTED);



	// mark the service as pending startup and then started (could nove that after thread created)

	processGlobals->updateServiceStatus(SERVICE_START_PENDING);

	processGlobals->updateServiceStatus(SERVICE_RUNNING);


	// start the background service thread and wait until the service ends

    HANDLE hServiceThread = createThread(StartServiceThread, "StartServiceThread");

	if (hServiceThread != NULL)
	{
		ServiceLogger::write("Service Main waiting for event flags to be set\n");

		// wait (indefinitely) until start and stop event flags are both set

		processGlobals->waitForBothEvents();

		// don't forget to clean up the thread handle now (thread has ended)

		CloseHandle(hServiceThread);
	}

	// log that the service has stopped - normally or not

	if (processGlobals->getServiceStartedSuccessfully())
	{
		processGlobals->logServiceEvent(EVENT_SERVICE_STOPPED);
	}
	else
	{
		processGlobals->logServiceEvent(EVENT_START_FAILED);
	}

	// inform service manager that service has now stopped, as requested

	processGlobals->setStatusWaitHint(0);
	processGlobals->updateServiceStatus(SERVICE_STOPPED);

	ServiceLogger::write("Service Main cleanup and end (service stopped)\n");

	// release any event and event source handles

	processGlobals->cleanUp();
	delete processGlobals;
	processGlobals = NULL;
}




static void WINAPI ServiceControlHandler(DWORD opcode)
{
	ServiceLogger::write("ServiceControlHandler invoked\n");

	switch (opcode)
	{
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
		{
			ServiceLogger::write("Service control stop/shutdown requested\n");

			// Tell the service manager that stop is pending, and may take longer than the 20-sec shutdown

			processGlobals->setStatusWaitHint(SERVICE_SHUTDOWN_MSECS);
			processGlobals->updateServiceStatus(SERVICE_STOP_PENDING);

			// Invoke the stop method in another thread, at higher priority level
			HANDLE hStopThread = createThread(StopServiceThread, "StopServiceThread");
			if (hStopThread != NULL)
			{
				SetThreadPriority(hStopThread, THREAD_PRIORITY_ABOVE_NORMAL);
				CloseHandle(hStopThread); // this handle is no longer needed
			}

			// Create a thread to stop the service in a number of seconds, if it does not stop within that time
			HANDLE hTimeoutThread = createThread(TimeoutStopThread, "TimeoutStopThread");

			if (hTimeoutThread != NULL)
			{
				CloseHandle(hTimeoutThread); // this handle is not needed
			}
			else
			{
				// if timeout thread could not be created, stop the service now to be safe
				processGlobals->setBothEvents();
			}
			break;
		}
    case SERVICE_CONTROL_PAUSE:
    case SERVICE_CONTROL_CONTINUE:
    case SERVICE_CONTROL_INTERROGATE:
	default:
		{
			ServiceLogger::write("Service control pause/continue/interrogate requested\n");
			processGlobals->setServiceStatus();
		}
	}
}



static HANDLE createThread(LPTHREAD_START_ROUTINE threadFunction, const char* threadName)
{

	ServiceLogger::write("Creating thread (");
	ServiceLogger::write(threadName);
	ServiceLogger::write(")\n");

	DWORD threadId = 0;

	HANDLE hThread = CreateThread(NULL, 0, threadFunction, NULL, 0, &threadId);

	if (hThread == NULL)
	{
		ServiceLogger::write("Failed to create thread\n");
		logFunctionError(processGlobals->getEventSource(), "CreateThread");
	}

	return hThread;
}




static DWORD WINAPI StartServiceThread(LPVOID lpParam)
{
	ServiceLogger::write("Start Service Thread invoked\n");

	// get service configuration parameters from registry

	const ServiceParameters* serviceParams = processGlobals->getServiceParameters();

	if (serviceParams == NULL)
	{
		ServiceLogger::write("Start Service Thread failed to get service parameters\n");
		processGlobals->setBothEvents();
		return -1;
	}

	// if a current operating directory is specified, change to that location now

	if (serviceParams->currentDirectory != NULL)
	{
		ServiceLogger::write("Start Service Thread setting current directory to '");
		ServiceLogger::write(serviceParams->currentDirectory);
		ServiceLogger::write("'\n");

		if (!SetCurrentDirectory(serviceParams->currentDirectory))
		{
			ServiceLogger::write("Start Service Thread failed to set directory\n");
			logFunctionError(processGlobals->getEventSource(), "SetCurrentDirectory"); //TODO - include directory string?
			processGlobals->setBothEvents();
			delete serviceParams;
			return -1;
		}
	}

	// if there is a path extension, set that up for the local environment

	if (serviceParams->pathExt != NULL)
	{
		ServiceLogger::write("Start Service Thread setting path extension '");
		ServiceLogger::write(serviceParams->pathExt);
		ServiceLogger::write("'\n");

		if (!setExtendedPath(serviceParams->pathExt))
		{
			ServiceLogger::write("Start Service Thread failed to set path extension\n");
			processGlobals->setBothEvents();
			delete serviceParams;
			return -1;
		}
	}




	ServiceLogger::write("Start Service Thread starting the java service...\n");

	//Run the java service.
	bool startedSuccessfully = StartJavaService(processGlobals->getEventSource(),
												serviceParams);

	ServiceLogger::write(startedSuccessfully ? "Start Service Thread started ok\n"
								 : "Start Service Thread failed\n");

	processGlobals->setServiceStartedSuccessfully(startedSuccessfully);

	delete serviceParams;


	//Tell the main thread that the service is no longer running.
	if (startedSuccessfully)
	{
		processGlobals->setStartEvent();
	}
	else
	{
		processGlobals->setBothEvents();
	}

	return 0;
}



static bool setExtendedPath(const char* pathExt)
{

	// check to see if anything needs doing at all
	if ((pathExt == NULL) || (strlen(pathExt) == 0))
	{
		return true; // do nothing, but treat as success
	}

	//Get the length of the current path.
	const DWORD currentPathLength = GetEnvironmentVariable("PATH", NULL, 0);

	//Allocate a buffer big enough for the current path and the new path.
	const int pathBufferLen = currentPathLength + strlen(pathExt) + 2;
	char *currentPath = (char *)malloc(pathBufferLen);

	if (currentPath == NULL)
	{
		logFunctionMessage(processGlobals->getEventSource(), "malloc", "Could not allocate memory");
		return false;
	}

	//Zero out the buffer so string functions will work.
	memset(currentPath, 0, pathBufferLen);

	//Copy the current path into the buffer.
	if (GetEnvironmentVariable("PATH", currentPath, currentPathLength) > currentPathLength)
	{
		logFunctionMessage(processGlobals->getEventSource(), "GetEnvironmentVariable", "Path length has changed between calls");
		return false;
	}

	//If there was a path already and there is a new path, append a seperator.
	if (strlen(currentPath) > 0 && strlen(pathExt) > 0)
	{
		strcat(currentPath, ";");
	}

	//Append the new path.
	strcat(currentPath, pathExt);

	//Set the new path.
	if (SetEnvironmentVariable("PATH", currentPath) == 0)
	{
		//Log the error to the event log.
		logFunctionError(processGlobals->getEventSource(), "SetEnvironmentVariable");
		return false;
	}

	return true; // path environment variable extended with specified addition(s)
}




static DWORD WINAPI StopServiceThread(LPVOID lpParam)
{
	ServiceLogger::write("Stop Service Thread invoked\n");

	// get service configuration parameters from registry

	const ServiceParameters* serviceParams = processGlobals->getServiceParameters();

	if (serviceParams == NULL)
	{
		processGlobals->setBothEvents();
		return -1;
	}

	if (serviceParams->stopClass != NULL)
	{
		//Stop the java service.
		if (StopJavaService(processGlobals->getEventSource(), 
							serviceParams))
		{
			ServiceLogger::write("Stop Service Thread stopped java service\n");

			processGlobals->setServiceStoppedSuccessfully(true); // avoid timeout log
			processGlobals->setStopEvent();
		}
		else //NOTE - may be incorrect, if stop class/method not fully specified...
		{
			ServiceLogger::write("Stop Service Thread failed to stop java service\n");

			//If there was an error running the stop method, stop the service.
			processGlobals->logServiceEvent(EVENT_STOP_FAILED);
			processGlobals->setServiceStoppedSuccessfully(true); // avoid timeout log
			processGlobals->setBothEvents();
		}

	}
	else
	{
		ServiceLogger::write("Stop Service Thread not configured to stop via class function, setting exit flags now\n");

		//There is no stop class, tell the service to stop.
		processGlobals->setServiceStoppedSuccessfully(true); // avoid timeout log
		processGlobals->setBothEvents();
	}

	return 0;
}




static DWORD WINAPI TimeoutStopThread(LPVOID lpParam)
{
	ServiceLogger::write("Timeout Stop Thread invoked\n");

	//Sleep for required number of seconds.
	Sleep(SHUTDOWN_TIMEOUT_MSECS);

	if (!processGlobals->getServiceStoppedSuccessfully())
	{
		processGlobals->logServiceEvent(EVENT_STOP_TIMEDOUT);
	}

	// Ensure the main thread is notified that the service is no longer running.
	processGlobals->setBothEvents();

	return 0;
}




//
// Globally-accessed function used on shutdown
//
void ExitHandler(int code)
{
	ServiceLogger::write("Exit Handler invoked\n");

	//TODO - use specific message formatting function, instead of sprintf

	//Log the code to the event log.
	char exitMessage[256];
	sprintf(exitMessage, "The Java Virtual Machine has exited with a code of %d, the service is being stopped.", code);

	logEventMessage(processGlobals->getEventSource(), exitMessage, (code == 0 ? EVENT_GENERIC_INFORMATION : EVENT_GENERIC_ERROR));

	//Tell the main thread that the service is no longer running.
	processGlobals->setBothEvents();

	Sleep(EXIT_HANDLER_TIMEOUT_MSECS);
}
