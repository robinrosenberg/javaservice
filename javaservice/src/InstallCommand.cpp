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

#include <windows.h>
#include <iostream.h>
#include "InstallCommand.h"
#include "ServiceParameters.h"
#include "RegistryHandler.h"


/*
 * Construct instance of service installation command, parsing all arguments
 */
InstallCommand::InstallCommand(const char* _serviceName, int servArgc, char* servArgv[])
: ServiceCommand(_serviceName)
, serviceParameters(NULL)
, parametersValid(false)
{
	parametersValid = validateParameters(servArgc, servArgv);
}


InstallCommand::~InstallCommand()
{
	if (serviceParameters != NULL)
	{
		delete serviceParameters;
	}
}


/*
 * Execute the 'install service' command
 */
int InstallCommand::execute()
{

	int commandStatus = COMMAND_FAILURE;
	bool lookupError = true;

	if (!parametersValid)
	{
		cerr << "JavaService install command parameters not valid, or incomplete";
	}
	else if (isServiceInstalled(lookupError) && !lookupError)
	{
		cerr << "The " << getServiceName() << " service is already installed";
	}
	else if (lookupError)
	{
		cerr << "Error while checking to see if " << getServiceName() << " service is installed" << endl;
		printLastError();
	}
	else
	{
		bool installed = installService();

		if (installed)
		{
			commandStatus = COMMAND_SUCCESS; // success, even if saving config fails

			if (!saveServiceConfig())
			{
				cerr << "Error saving " << getServiceName() << "  service configuration (service installed)" << endl;
				printLastError();
			}
		}
		else
		{
			cerr << "Error attempting to install " << getServiceName() << "  service" << endl;
			printLastError();
		}
	}

	return commandStatus;
}


bool InstallCommand::validateParameters(int servArgc, char* servArgv[])
{
	serviceParameters = new ServiceParameters();

	return serviceParameters->loadFromArguments(servArgc, servArgv);
}


bool InstallCommand::installService()
{
	bool installed = false;

	if (createService(*serviceParameters))
	{
		// successful installation, output result information

		const char* startType = serviceParameters->isAutoStart() ? "automatic" : "manual";

		if (serviceParameters->getDependency() == NULL)
		{
			cout << "The " << getServiceName() << " " << startType << " service was successfully installed" << endl;
		}
		else
		{
			cout << "The " << getServiceName() << " " << startType << " service was successfully installed, depends on "
				 << serviceParameters->getDependency() << " service." << endl;
		}

		installed = true;
	}
	else
	{
		cerr << "Error attempting to install the " << getServiceName() << " service" << endl;
		printLastError();
	}


	return installed;

}


bool InstallCommand::saveServiceConfig()
{
	RegistryHandler* registryHandler = new RegistryHandler(getServiceName());

	bool saved = registryHandler->writeServiceParams(*serviceParameters);

	delete registryHandler;

	return saved;
}
