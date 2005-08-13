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

#include "JsString.h"

#include <malloc.h>
#include <string.h>
#include <iostream.h>


// default constructor
// does nothing other than initialising null char pointer
JsString::JsString()
: charPtr(NULL)
{

}


// constant string constructor, copies supplied string
JsString::JsString(const char* inputString)
: charPtr(NULL)
{
	if (inputString != NULL)
	{
		setBuffer(inputString);
	}
}


// copy constructor, copy supplied string
JsString::JsString(const JsString& other)
: charPtr(NULL)
{
	setBuffer(other.getCharPtr());
}


// destructor, performs any require cleanup
JsString::~JsString()
{
	freeBuffer();
}


// get read-only accessor for internal string buffer (may be null)
const char* JsString::getCharPtr() const
{
	return charPtr;
}



// return length of string buffer
int JsString::getLength() const
{

	return (charPtr != NULL) ? strlen(charPtr) : 0;
}


// release any data held in the string buffer
void JsString::clear()
{
	freeBuffer();
}



// return simple 'const char*' pointer to string buffer (or null)
JsString::operator const char*() const
{
	return getCharPtr();
}


// copy specified string contents to buffer
JsString& JsString::operator=(const JsString& other)
{
	if (&other != this) // avoid copying to self
	{
		setBuffer(other.getCharPtr());
	}
	return *this;
}


// copy specified string contents to buffer
JsString& JsString::operator=(const char* other)
{
	setBuffer(other);
	return *this;
}


// release any memory allocated for string buffer
void JsString::freeBuffer()
{

	if (charPtr != NULL)
	{
		free((void*)charPtr);
		charPtr = NULL;
	}
}



// function to allocate memory block for string storage of specified length
// (length to include space for the null terminator at the end of the string)
char* JsString::allocCharBuffer(int length)
{
	return (char*)calloc(1, length);
}


// store specified string in buffer, freeing any existing content
void JsString::setBuffer(const char* inputString)
{
	freeBuffer();

	int inputLength = (inputString != NULL) ? strlen(inputString) : 0;
	if (inputLength > 0)
	{
		char* tmpPtr = allocCharBuffer(inputLength + 1);
		strcpy(tmpPtr, inputString);
		charPtr = tmpPtr;
	}
}


// output stream handler, create 'safe' output of string contents
ostream& operator<< (ostream& os, const JsString& str)
{
	const char* txt = str.getCharPtr();
	if (txt != NULL)
	{
		os << txt;
	}
	return os;
}

