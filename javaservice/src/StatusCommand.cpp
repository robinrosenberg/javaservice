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
#include "StatusCommand.h"
#include "ServiceParameters.h"


/*
 * Execute the service status enquiry command
 */
int StatusCommand::execute()
{

	bool lookupError = false;
	bool isInstalled = isServiceInstalled(lookupError);

	if (lookupError)
	{
		cout << "Failed to check for current installation status of service: " << getServiceName() << endl;
	}
	else if (!isInstalled)
	{
		cout << "Service '" << getServiceName() << "' is not currently installed" << endl;
	}
	else
	{
		cout << "Service '" << getServiceName() << "' is currently installed" << endl;


		const char* serviceStatusText = getServiceStatusText();

		cout << getServiceName() << " " << serviceStatusText << endl;

		ServiceParameters* serviceParams = new ServiceParameters();

		bool gotConfig = serviceParams->readFromRegistry(getServiceName());

		delete serviceParams;

		if (gotConfig)
		{
			cout << "Service configuration parameters are defined in the registry" << endl;
		}
		else
		{
			cout << "Service configuration parameters are not defined in the registry" << endl;
		}
	}

	cout << endl << flush;

	return lookupError ? COMMAND_FAILURE : COMMAND_SUCCESS;
}
