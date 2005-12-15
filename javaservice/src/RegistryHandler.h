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

#ifndef __REGISTRY_HANDLER_H__
#define __REGISTRY_HANDLER_H__

#include <windows.h> // HKEY


// forward class reference
class ServiceParameters;



class RegistryHandler
{
public:

	RegistryHandler(const char* serviceName);

	~RegistryHandler();
	
	bool writeServiceParams(const ServiceParameters& serviceParams);

	bool readServiceParams(ServiceParameters& serviceParams);

	bool deleteServiceParams();

protected:

	HKEY createRegKey(const char* regKeyName);

	HKEY RegistryHandler::openRegKey(const char* regKeyName);

	bool deleteRegKey(const char* regKeyName);

	bool storeRegValue(HKEY hRegKey, const char* entryKey, int valueType, BYTE* entryValue, int valueLen);

	bool storeRegValueString(HKEY hRegKey, const char* entryKey, const char* entryValue);

	bool storeRegValueDword(HKEY hRegKey, const char* entryKey, const int entryValue);

	bool storeRegValueBoolean(HKEY hRegKey, const char* entryKey, const bool entryValue);

	bool storeRegValueExpString(HKEY hRegKey, const char* entryKey, const char* entryValue);

	bool getRegValueString(HKEY hRegKey, const char* entryKey, char** entryValue);

	bool getRegValueDword(HKEY hRegKey, const char* entryKey, int* entryValue);

	bool getRegValueBoolean(HKEY hRegKey, const char* entryKey, bool* entryValue);

private:

	bool setupAnonymousLogging();

	bool setupServiceLogging();

	bool setupLoggingEntries(const char* regKeyName);

	bool getJavaLibraryEntry(char** jvmLibraryEntry);

private:

	const char* serviceName;
	const char* serviceKeyName;
	const char* loggingKeyName;

	void createServiceKeyName(); // called by ctor
	void createLoggingKeyName(); // called by ctor
	void deleteKeyName(const char*& keyNameRef);


	// declare but do not implement default ctor, copy ctor and copy operator
	RegistryHandler();
	RegistryHandler(const RegistryHandler& other);
	RegistryHandler operator=(const RegistryHandler& other);

};

#endif // __REGISTRY_HANDLER_H__
