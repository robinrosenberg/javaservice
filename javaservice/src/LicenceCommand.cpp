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

#include <iostream.h>
#include "LicenceCommand.h"

//
// Local constant data declarations
//

static const char* const COPY_TEXT
  = "\n (c) Copyright 2006 Multiplan Consultants Limited.\n";

static const char* const LGPL_TEXT
  = "\n This library is free software; you can redistribute it and/or"
	"\n modify it under the terms of the GNU Lesser General Public"
	"\n License as published by the Free Software Foundation; either"
	"\n version 2.1 of the License, or (at your option) any later version."
	"\n"
	"\n This library is distributed in the hope that it will be useful,"
	"\n but WITHOUT ANY WARRANTY; without even the implied warranty of"
	"\n MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU"
	"\n Lesser General Public License for more details."
	"\n"
	"\n You should have received a copy of the GNU Lesser General Public"
	"\n License along with this library; if not, write to the Free Software"
	"\n Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n";

static const char* const INFO_TEXT
  = "\n Information about the JavaService software is available at the ObjectWeb"
	"\n web site. Refer to http://javaservice.objectweb.org for more details.\n";


/*
 * Execute the 'show licence' command, display the LGPL licence text
 */
int LicenceCommand::execute()
{

	printLicence();
	return COMMAND_SUCCESS;
}


/*
 * Output the LGPL licence and additional text to the console (stdout)
 */
void LicenceCommand::printLicence()
{
	// output text of LGPL Licence (aka License for 'Mericans)

	cout << COPY_TEXT << endl
		 << LGPL_TEXT << endl
		 << INFO_TEXT << endl << flush;

}
