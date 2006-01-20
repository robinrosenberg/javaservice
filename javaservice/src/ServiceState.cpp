/*
 * JavaService - Windows NT Service Daemon for Java applications
 *
 * Copyright (C) 2006 Multiplan Consultants Ltd.
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

#include "ServiceState.h"

//
// Class wrapper for maintenance and notification of service status
//

//
// Default ctor
//
ServiceState::ServiceState()
: serviceHandle(NULL)
{
	// set up initial values in service status structure

	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;
}


bool ServiceState::registerHandler(const char* serviceName, LPHANDLER_FUNCTION serviceHandlerFunction)
{
	serviceHandle = RegisterServiceCtrlHandler(serviceName, serviceHandlerFunction);

	return (serviceHandle != NULL);
}

//
// Dtor
//
ServiceState::~ServiceState()
{
}



void ServiceState::updateServiceStatus(int currentState, int waitHint)
{
	// update hint value, set the specified status value and notify the service manager
	serviceStatus.dwWaitHint = waitHint;
	updateServiceStatus(currentState);

}


void ServiceState::updateServiceStatus(int currentState)
{
	// set the specified status value and notify the service manager
	serviceStatus.dwCurrentState = currentState;
	notifyServiceStatus();
}


void ServiceState::notifyServiceStatus()
{
	// notify the service manager of current service status
	// (must be called for all service messages, even if status unchanged)

	if (serviceHandle != NULL) // do nothing if not initialised
	{
		BOOL sts = SetServiceStatus(serviceHandle, &serviceStatus);

		if (!sts)
		{
			// log event error
		}
	}
}
