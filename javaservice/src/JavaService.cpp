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
#include "ServiceHandler.h"
#include "CommandFactory.h"
#include "CommandInterface.h"

//
// JavaService program entry point
//
// This function is either invoked from the command line with a range of arguments,
// or it gets invoked in the background as a service (with no parameters/arguments).
// The argument list is parsed for validity and to action the desired command, or to
// output an error message. If no arguments, assume process is running as a service.
//
int main(int argc, char* argv[])
{

	// parse the command parameter list to determine the action to be performed

	int mainStatus = -1; // default to error indication (if anyone cares to check it)

	CommandInterface* command = CommandFactory::createCommand(argc, argv);

	if (command != NULL)
	{
		mainStatus = command->execute();
		delete command; // ensure temporary command object is released
	}

	return mainStatus;
}
