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
#include "ServiceCommand.h"
#include "ServiceParameters.h"

//
// local function references
//
static const char* getDependencyString(const char* dependsOn);

static void deleteDependencyString(const char* dependency);




void ServiceCommand::printErrorMessage(int errorCode)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
	cerr << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}


SC_HANDLE ServiceCommand::openSCManager(int wot)
{
	return OpenSCManager(NULL, NULL, wot);
}


SC_HANDLE ServiceCommand::openService(SC_HANDLE hSCM, int wot)
{
	return OpenService(hSCM, getServiceName(), wot);
}


bool ServiceCommand::isServiceInstalled(bool& lookupFailed)
{

	bool isInstalled = false;
	lookupFailed = true; // error indication by default

	SC_HANDLE hSCM = openSCManager(GENERIC_READ);

	if (hSCM != NULL)
	{
		// try to open the service, check for expected error if that fails

		SC_HANDLE hService = openService(hSCM, SERVICE_QUERY_CONFIG);

		if (hService != NULL)
		{
			isInstalled = true;		// service opened, so it is installed
			lookupFailed = false;	// successful lookup
			CloseServiceHandle(hService);
		}
		else if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
		{
			lookupFailed = false;	// lookup failed, expected error
		}
		else
		{
			lookupFailed = true;	// unexpected error, run-time failure
		}

		CloseServiceHandle(hSCM);
	}

	return isInstalled;
}



int ServiceCommand::getServiceStatus(bool& lookupFailed)
{

	SERVICE_STATUS serviceStatus;
	memset(&serviceStatus, sizeof(serviceStatus), 0);
	serviceStatus.dwCurrentState = 0;

	lookupFailed = true; // error indication by default

	SC_HANDLE hSCM = openSCManager(GENERIC_READ);

	if (hSCM != NULL)
	{
		// try to open the service, check for expected error if that fails

		SC_HANDLE hService = openService(hSCM, SERVICE_QUERY_STATUS);

		if (hService != NULL)
		{
			BOOL queryStatus = QueryServiceStatus(hService, &serviceStatus);

			lookupFailed = (queryStatus != TRUE); // unsuccessful lookup?

			CloseServiceHandle(hService);
		}
		else
		{
			lookupFailed = true;	// unexpected error, run-time failure
		}

		CloseServiceHandle(hSCM);
	}

	return serviceStatus.dwCurrentState;

}


const char* ServiceCommand::getServiceStatusText()
{
	const char* serviceStatusText = NULL;

	bool lookupError = true;

	int serviceStatus = getServiceStatus(lookupError);

	if (lookupError)
	{
		printLastError();
		serviceStatusText = "Error determining current status of service";
	}
	else
	{
		switch (serviceStatus) // values below defined in winsvc.h
		{
		case SERVICE_STOPPED:
			serviceStatusText = "Service is stopped";
			break;
		case SERVICE_START_PENDING:
			serviceStatusText = "Service is starting";
			break;
		case SERVICE_STOP_PENDING:
			serviceStatusText = "Service is stopping";
			break;
		case SERVICE_RUNNING:
			serviceStatusText = "Service is running";
			break;
		case SERVICE_CONTINUE_PENDING:
			serviceStatusText = "Service is continuing from a pause";
			break;
		case SERVICE_PAUSE_PENDING:
			serviceStatusText = "Service is beginning to pause";
			break;
		case SERVICE_PAUSED:
			serviceStatusText = "Service is paused";
			break;
		default:
			cerr << "Service current status value " << serviceStatus << " is undefined" << endl;
			serviceStatusText = "Service state is not recognised";
			break;
		}
	}

	return serviceStatusText;
}


bool ServiceCommand::createService(const ServiceParameters& serviceParams)
{
	bool installedOk = false;

	SC_HANDLE hSCM = openSCManager(SC_MANAGER_CREATE_SERVICE);

	if (hSCM != NULL)
	{

		// get the file path of the running executable
		char filePath[MAX_PATH];
		GetModuleFileName(NULL, filePath, sizeof(filePath));

		// if service dependency specified, set up correct parameter type here
		const char* dependency = getDependencyString(serviceParams.getDependency());

		// set up automatic or manual service startup mode
		DWORD dwStartType = serviceParams.isAutoStart() ? SERVICE_AUTO_START : SERVICE_DEMAND_START;

		// attempt to create the service
		SC_HANDLE hService = CreateService(hSCM,						// hSCManager
										   getServiceName(),			// lpServiceName
										   getServiceName(),			// lpDisplayName
										   SERVICE_ALL_ACCESS,			// dwDesiredAccess
										   SERVICE_WIN32_OWN_PROCESS,	// dwServiceType
										   dwStartType,					// dwStartType
										   SERVICE_ERROR_NORMAL,		// dwErrorControl
										   filePath,					// lpBinaryPathName
										   NULL,						// lpLoadOrderGroup
										   NULL,						// lpdwTagId
										   dependency,					// lpDependencies
										   NULL,						// lpServiceStartName
										   NULL);						// lpPassword

		if (hService != NULL)
		{
			installedOk = true;
			CloseServiceHandle(hService);
		}

		deleteDependencyString(dependency);

		CloseServiceHandle(hSCM);
	}


	return installedOk;
}


static const char* getDependencyString(const char* dependsOn)
{
	char* dependency = NULL;

	if (dependsOn != NULL)
	{
		// set up dependency parameter with extra double null-terminator
		int dependencyLen = strlen(dependsOn) + 3;
		dependency = new char[dependencyLen];
		memset( dependency, 0, dependencyLen );
		strcpy( dependency, dependsOn );
	}

	return dependency;
}


// ensure correct delete mode specified, to match the 'new' above
static void deleteDependencyString(const char* dependency)
{
	if (dependency != NULL)
	{
		delete[] (void*)dependency;
	}
}

bool ServiceCommand::deleteService()
{
	bool uninstalled = false;

	SC_HANDLE hSCM = openSCManager(SC_MANAGER_ALL_ACCESS);

	if (hSCM != NULL)
	{
		SC_HANDLE hService = openService(hSCM, DELETE);

		if (hService != NULL)
		{

			// note, this 'marks the service for deletion', so may not apply immediately
			BOOL deleteStatus = DeleteService(hService);
			uninstalled = (deleteStatus == TRUE);

			CloseServiceHandle(hService);
		}

		CloseServiceHandle(hSCM);
	}

	return uninstalled;
;
}

