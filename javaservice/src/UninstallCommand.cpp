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
#include "UninstallCommand.h"
#include "RegistryHandler.h"


/*
 * Execute the 'uninstall service' command
 */
int UninstallCommand::execute()
{
	int commandStatus = COMMAND_FAILURE;
	bool lookupError = true;
	bool serviceInstalled = isServiceInstalled(lookupError);

	if (lookupError)
	{
		cerr << "Error while checking to see if " << getServiceName() << " service is installed" << endl;
		printLastError();
	}
	else if (!serviceInstalled)
	{
		cout << "The " << getServiceName() << " service is not currently installed." << endl;
		commandStatus = COMMAND_SUCCESS; // treat as success, end result is the same
	}
	else if (uninstallService()) // delete service and registry entries
	{
		commandStatus = COMMAND_SUCCESS;
	}
	else
	{
		cerr << "Error attempting to uninstall " << getServiceName() << " service" << endl;
		printLastError();
	}

	return commandStatus;
}

/*
 * Remove service installation from system setup
 */
bool UninstallCommand::uninstallService()
{

	bool uninstalled = deleteService();

	if (uninstalled)
	{
		cout << "Successfully uninstalled service " << getServiceName() << endl;

		if (false) // don't need to do this, uses service section of registry, so automatically deleted
		{
			if (!deleteServiceConfig())
			{
				cerr << "Error deleting " << getServiceName() << " service configuration (service uninstalled)" << endl;
				printLastError();
			}
		}
	}
	
	return uninstalled; // ignore any error in cleaning up registry entries
}

/*
 * Delete configuration details for this service
 * Note that this is not required whilst config entries in the registry
 * are defined under the 'services control set' section, as they are then
 * deleted automatically from the registry when the service is removed.
 */
bool UninstallCommand::deleteServiceConfig()
{
	RegistryHandler* registryHandler = new RegistryHandler(getServiceName());

	bool deleted = registryHandler->deleteServiceParams();

	delete registryHandler;

	return deleted;
}
