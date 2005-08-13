/*
 * JavaService Library - Windows NT Service Daemon for Java applications
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
 */

#include <string.h>
#include <stdio.h>
#include <iostream.h>

#include "JsString.h"


static const JsString programName("JavaService Library Test");

void main(int argc, char* argv[])
{
	printf(programName);
	printf("\n");

	const JsString hello = "Hello World\n";
	printf(hello);
	fflush(stdout);

	const JsString uno("Uno");
	const JsString dos("Dos");
	const JsString tres("Tres");

	cout << endl << uno << ", " << dos << ", " << tres << endl << flush;

	printf("\n");
	printf(uno);
	printf("\n");
	printf(dos);
	printf("\n");
	printf(tres);
	printf("\n");

	printf("\n");
	const JsString str1(uno);
	printf(str1);
	printf("\n");
	const JsString str2(dos);
	printf(str2);
	printf("\n");
	JsString str3;
	str3 = tres;
	printf(str3);
	printf("\n");

	printf("\n");
	str3 = str3;
	printf(str3);
	printf("\n");
	const char* cc2 = str2;
	printf(cc2);
	printf("\n");
	const char* const cc1 = str1;
	printf(cc1);
	printf("\n");

	JsString* str = new JsString("\nThat's all folks!!!\n");
	printf(*str);
	delete str;
	str = NULL;

	str = new JsString();
	cout << endl << "Empty JsString ctor '" << *str << "'" << endl << flush;

	(*str) = "";
	cout << endl << "Copied empty string '" << *str << "'" << endl << flush;

	(*str) = "xyzzy";
	cout << endl << "Copied non-empty string '" << *str << "'" << endl << flush;

	(*str) = NULL;
	cout << endl << "Copied null string '" << *str << "'" << endl << flush;

	delete str;
	str = NULL;

}