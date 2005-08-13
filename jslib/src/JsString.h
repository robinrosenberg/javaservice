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

#ifndef __JS_STRING_H__
#define __JS_STRING_H__


/*
 * This is a 'very simple' string class for use by JavaService C++ code.
 *
 * It makes no attempt at any performance-saving optimisations at all,
 * since strings in JavaService are basically assigned and freed, with
 * some string concatenation performed.
 *
 * This code would not be recommended for use in any application that
 * requires string processing or copying during general execution.
 */
class JsString
{
public:

	// default constructor, does nothing
	JsString();

	// constant string constructor, copies supplied string
	JsString(const char* inputString);

	// copy constructor, copy supplied string
	JsString(const JsString& other);

	// destructor, performs any require cleanup
	~JsString();

	// get read-only accessor for internal string buffer (may be null)
	const char* getCharPtr() const;

	// return length of string buffer
	int getLength() const;

	// release any data held in the string buffer
	void clear();


	// return simple 'const char*' pointer to string buffer (or null)
	operator const char*() const;

	// copy operator, copy specified string contents to buffer
	JsString& operator=(const JsString& other);

	// copy operator, copy specified string contents to buffer
	JsString& operator=(const char* other);

private:

	// character pointer for any locally-held string data
	const char* charPtr;

	// release any memory allocated for string buffer
	void freeBuffer();

	// store specified string in buffer, freeing any existing content
	void setBuffer(const char* inputString);


	// function to allocate memory block for string storage of specified length
	// (length to include space for the null terminator at the end of the string)
	char* allocCharBuffer(int length);


};


// output stream handler, create 'safe' output of string contents

class ostream;

ostream& operator<< (ostream& os, const JsString&);

#endif // __JS_STRING_H__