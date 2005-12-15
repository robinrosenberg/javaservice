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

#include <iostream.h>
#include "CommandFactory.h"
#include "CommandInterface.h"
#include "HelpCommand.h"
#include "VersionCommand.h"
#include "LicenceCommand.h"
#include "StatusCommand.h"
#include "QueryCommand.h"
#include "InstallCommand.h"
#include "UninstallCommand.h"
#include "DaemonCommand.h"

//
// Local constant definitions
//

// command-line argument index values
#define PROGRAM_NAME_ARG 0
#define COMMAND_VERB_ARG 1
#define COMMAND_PARAM_ARG 2

/*
 * Given command-line input parameters, perform required level of parsing to
 * create a CommandInterface object that can in turn be executed by the program.
 */
CommandInterface* CommandFactory::createCommand(int argc, char* argv[])
{

	// parse the argument list and create command object accordingly

	CommandInterface* command = NULL;
	
	// if no command-line parameters are present, then assumption is that
	// the application has been invoked as a background 'daemon' service
	// (there is no straightforward mechanism to find out otherwise)

	if (argc <= 1) // no parameters, other than program name?
	{
		command = new DaemonCommand(); // assume being run as a service
	}
	else
	{
		// the command verb comes first in the argument list, after program name

		const char* commandVerb = argv[COMMAND_VERB_ARG];

		int paramCount = argc - COMMAND_PARAM_ARG;	// count of command parameters after the verb

		// check up front that the verb is prefixed with the hyphen character
		if ((strlen(commandVerb) < 2) || (commandVerb[0] != '-'))
		{
			command = NULL; // error message and default help command handled below
		}
		else if (strcmp(commandVerb, "-version") == 0)
		{
			command = new VersionCommand();
		}
		else if ((strcmp(commandVerb, "-licence") == 0)
			  || (strcmp(commandVerb, "-license") == 0)) // accept both spellings, for convenience
		{
			command = new LicenceCommand();
		}
		else if ((strcmp(commandVerb, "-help") == 0)
			  || (strcmp(commandVerb, "-?") == 0)
			  || (strcmp(commandVerb, "-usage") == 0)) // alternative forms of help command
		{
			// extended help may be provided if further argument(s) supplied

			if (paramCount > 0)
			{
				command = new HelpCommand(paramCount, &argv[COMMAND_PARAM_ARG]);
			}
			else
			{
				command = new HelpCommand();
			}
		}
		else if (paramCount > 0)
		{
			// all other commands are first followed by the service name,
			// so get that for use when creating any command objects and
			// decrement the count of remaining command parameters

			const char* serviceName = argv[COMMAND_PARAM_ARG];
			paramCount--;

			if (strcmp(commandVerb, "-install") == 0)
			{
				command = new InstallCommand(serviceName, paramCount, &argv[COMMAND_PARAM_ARG + 1]);
			}
			else if (strcmp(commandVerb, "-uninstall") == 0)
			{
				command = new UninstallCommand(serviceName);
			}
			else if (strcmp(commandVerb, "-status") == 0)
			{
				command = new StatusCommand(serviceName);
			}
			else if ((strcmp(commandVerb, "-queryconfig") == 0)
				  || (strcmp(commandVerb, "-query") == 0)) // two altenative forms
			{
				command = new QueryCommand(serviceName);
			}
		}

	}

	// if no valid command object created by this stage, then the string
	// specified by the user was not known, so create default help command

	if (command == NULL)
	{
		cerr << endl << "JavaService command not recognised or not valid (showing help)" << endl;
		command = new HelpCommand();
	}

	return command;
}
