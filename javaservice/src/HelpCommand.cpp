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
#include <string.h>
#include "HelpCommand.h"


// explicit constructor, may set up details for extended help info
HelpCommand::HelpCommand(int helpArgc, char* helpArgv[])
: helpTopic(NULL)
{
	// extended help may be listed for single topic (ignores other args)
	if ((helpArgc > 0) && (helpArgv != NULL))
	{
		helpTopic = helpArgv[0];

		if ((helpTopic != NULL) && (*helpTopic == '-'))
		{
			helpTopic++; // skip presence of any leading hyphen

			if (*helpTopic == '\0')
			{
				helpTopic = NULL; // no topic if just a hyphen seen
			}
		}
	}
}


int HelpCommand::execute()
{
	if (helpTopic == NULL)
	{
		printCommandSummary();
	}
	else // for now, ignore topic and list all detailed commands
	{
		printCommandDetails(helpTopic);
	}

	return COMMAND_SUCCESS;
}


/*
 * Output brief list of commands to the console (stdout)
 */
void HelpCommand::printCommandSummary() const
{

	cout << "\n\nJavaService (Windows NT Service Daemon) commands:-\n\n"
			"\t-version\n"
			"\t-licence\n"
			"\t-install service_name jvm service_options...\n"
			"\t-queryconfig service_name\n"
			"\t-status service_name\n"
			"\t-uninstall service_name\n"
			"\t-help\n"
			"\t-help topic\n"
		 << endl << flush;
}


void HelpCommand::printCommandDetails(const char* forTopic) const
{
	const char *commandDetails = NULL;

	if (!strncmp(forTopic, "version", 4))
	{
		commandDetails = "To display software version number:\n\n"
						 "JavaService -version";
	}
	else if (!strncmp(forTopic, "licence", 4))
	{
		commandDetails = "To display software licence details:\n\n"
						 "JavaService -licence\n"
						 "  or\n"
						 "JavaService -license";
	}
	else if (!strncmp(forTopic, "install", 4))
	{
		commandDetails = "To configure and install a background service:\n\n"
						 "JavaService -install service_name jvm_library [jvm_options]\n"
						 "\t-start start_class [-method start_method] [-params (start_parameters)]\n"
						 "\t[-stop start_class [-method stop_method] [-params (stop_parameters)]]\n"
						 "\t[-out out_log_file] [-err err_log_file]\n"
						 "\t[-current current_dir]\n"
						 "\t[-path extra_path]\n"
						 "\t[-depends other_service]\n"
						 "\t[-auto | -manual]\n"
						 "\t[-shutdown seconds]\n"
						 "\nWhere:\n"
						 "  service_name:\tThe name of the service.\n"
						 "  jvm_library:\tThe location of the JVM DLL used to run the service.\n"
						 "  jvm_option:\tAn option to use when starting the JVM, such as:\n"
						 "\t\t\t\"-Djava.class.path=c:\\classes\" or \"-Xmx128m\".\n"
						 "  start_class:\tThe class to load when starting the service.\n"
						 "  start_method:\tThe method to call in the start_class. default: main\n"
						 "  start_parameters:Parameter(s) to pass in to the start_method.\n"
						 "  stop_class:\tThe class to load when stopping the service.\n"
						 "  stop_method:\tThe method to call in the stop_class. default: main\n"
						 "  stop_parameters:\tParameter(s) to pass in to the stop_method.\n"
						 "  out_log_file:\tA file to redirect System.out into. (gets overwritten)\n"
						 "  err_log_file:\tA file to redirect System.err into. (gets overwritten)\n"
						 "  current_dir:\tThe current working directory for the service.\n"
						 "\t\t\tRelative paths will be relative to this directory.\n"
						 "  extra_path:\tPath additions, for native DLLs etc. (no spaces)\n"
						 "  other_service:\tSingle service name dependency, must start first.\n"
						 "  auto / manual:\tStartup automatic (default) or manual mode.\n"
						 "  seconds:\tTime for Java method shutdown processing before timeout.\n";
	}
	else if (!strncmp(forTopic, "queryconfig", 4))
	{
		commandDetails = "To show configuration of a background service:\n\n"
						 "JavaService -queryconfig service_name\n"
						 "  or\n"
						 "JavaService -query service_name\n"
						 "\nWhere:\n"
						 "  service_name:\tThe name of the service.";
	}
	else if (!strncmp(forTopic, "status", 4))
	{
		commandDetails = "To show current status of a background service:\n\n"
						 "JavaService -status service_name\n"
						 "\nWhere:\n"
						 "  service_name:\tThe name of the service.";
	}
	else if (!strncmp(forTopic, "uninstall", 4))
	{
		commandDetails = "To remove and uninstall a background service:\n\n"
						 "JavaService -uninstall service_name\n"
						 "\nWhere:\n"
						 "  service_name:\tThe name of the service.";
	}
	else
	{
		commandDetails = "Extended help available for the topics:\n\n"
						 "\tversion\n"
						 "\tlicence\n"
						 "\tinstall\n"
						 "\tqueryconfig\n"
						 "\tstatus\n"
						 "\tuninstall";
	}

	cout << endl << commandDetails << endl << flush;
}
