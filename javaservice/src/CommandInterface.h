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

#ifndef __COMMAND_INTERFACE_H__
#define __COMMAND_INTERFACE_H__


//
// Constant declarations to be used in exit status of command execute functions
//

// success normally indicated with a zero program exit status
#define COMMAND_SUCCESS 0
// failure indicated with a non-zero exit status,
// set greater than zero for ERRORLEVEL tests in batch files
#define COMMAND_FAILURE 666


/*
 * Abstract base class definition specifying functions to be implemented in all command classes
 */
class CommandInterface
{
public:

	// base class constructor, does nothing
	CommandInterface() {}

	// base class destructor, does nothing (virtual for inheritance)
	virtual ~CommandInterface() {};

	// execute the command - should return COMMAND_SUCCESS or COMMAND_FAILURE
	virtual int execute() = 0;

	// return the command (class) name as a printable string
	virtual const char* getCommandName() const = 0;

};

#endif // __COMMAND_INTERFACE_H__
