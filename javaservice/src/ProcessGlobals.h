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

#ifndef __PROCESS_GLOBALS_H__
#define __PROCESS_GLOBALS_H__

#include <windows.h>


class ServiceParameters; // forward reference




class ProcessGlobals
{
public:


	// accessor for creation of process global data singleton, checks for duplicate initialisation
	static ProcessGlobals* createInstance(const char* serviceName);

	// general accessor for use of process global data singleton, checks for prior initialisation
	static ProcessGlobals* getInstance();

	// release any event and event source handles and destroy the singleton
	static void destroyInstance();



	void waitForBothEvents();

	void setBothEvents();

	void setStartEvent();

	void setStopEvent();


	void logServiceEvent(int messageType);


	const char* getServiceName() const { return serviceName; }

	HANDLE getEventSource() const { return hEventSource; }

	const ServiceParameters* getServiceParameters() const { return serviceParams; }

	bool getServiceStartedSuccessfully() const { return serviceStartedSuccessfully; }
	void setServiceStartedSuccessfully(bool successful) { serviceStartedSuccessfully = successful; }

	bool getServiceStoppedSuccessfully() const { return serviceStoppedSuccessfully; }
	void setServiceStoppedSuccessfully(bool successful) { serviceStoppedSuccessfully = successful; }
	
private:

	// [singleton] static process data declarations used in operation of the service
	static ProcessGlobals* globalsInstance;


	ProcessGlobals();

	bool initialise(const char* serviceName);

	void cleanUp();

	~ProcessGlobals();




	HANDLE createEventHandle();
	
	void closeEventHandle(HANDLE* hEvent);

	bool loadServiceParameters();



	const char* serviceName;

	HANDLE hEventSource;

	ServiceParameters* serviceParams;

	bool serviceStartedSuccessfully;

	HANDLE hWaitForStart;
	HANDLE hWaitForStop;

	bool serviceStoppedSuccessfully;


	// copy ctor/operator functions defined but not implemented

	ProcessGlobals(const ProcessGlobals& other);
	ProcessGlobals& operator=(const ProcessGlobals& other);
};

#endif // __PROCESS_GLOBALS_H__
