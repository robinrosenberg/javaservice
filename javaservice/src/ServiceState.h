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

#ifndef __SERVICE_STATE_H__
#define __SERVICE_STATE_H__

#include <windows.h>

//
// Class wrapper for maintenance and notification of service status
//
class ServiceState
{
public:

	ServiceState();

	bool registerHandler(const char* serviceName, LPHANDLER_FUNCTION serviceHandlerFunction);

	~ServiceState();


	void updateServiceStatus(int currentState, int waitHint);

	void updateServiceStatus(int currentState);

	void notifyServiceStatus();

private:

	SERVICE_STATUS serviceStatus;

	SERVICE_STATUS_HANDLE serviceHandle;


	// copy ctor/operator functions defined but not implemented

	ServiceState(const ServiceState& other);
	ServiceState& operator=(const ServiceState& other);
};

#endif // __SERVICE_STATE_H__
