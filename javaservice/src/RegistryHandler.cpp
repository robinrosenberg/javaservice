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

#include <stdio.h> // sprintf
#include "RegistryHandler.h"
#include "ServiceParameters.h"
#include "VersionNo.h"

//
// Local function prototypes
//
static const char* createServiceKeyName(const char* serviceName);
static void deleteKeyName(const char* regKeyName);
static const char* createLoggingKeyName(const char* serviceName);

static LONG RegQueryValueExAllocate(HKEY hKey, LPCTSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE *lplpData, LPDWORD lpcbData);

//
// Local constant definitions
//

static const char* const SERVICE_REG_KEY_PREFIX = "SYSTEM\\CurrentControlSet\\Services\\";
static const char* const SERVICE_REG_KEY_SUFFIX = "\\Parameters";

static const char* const ANON_LOGGING_REG_KEY = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\JavaService";

static const char* const SERV_LOGGING_REG_KEY_PREFIX = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\";
	
static const DWORD EVENT_LOG_TYPES = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;



static const char* const REG_KEY_SW_VERSION = "JavaService Version"; // note, new value added for V2 software

static const char* const REG_KEY_JVM_LIBRARY = "JVM Library";
static const char* const REG_KEY_JVM_OPTION_COUNT = "JVM Option Count";
static const char* const REG_KEY_JVM_OPTION_NO_FMT = "JVM Option Number %d";
static const char* const REG_KEY_START_CLASS = "Start Class";
static const char* const REG_KEY_START_METHOD = "Start Method";
static const char* const REG_KEY_START_PARAM_COUNT = "Start Param Count";
static const char* const REG_KEY_START_PARAM_NO_FMT = "Start Param Number %d";
static const char* const REG_KEY_STOP_CLASS = "Stop Class";
static const char* const REG_KEY_STOP_METHOD = "Stop Method";
static const char* const REG_KEY_STOP_PARAM_COUNT = "Stop Param Count";
static const char* const REG_KEY_STOP_PARAM_NO_FMT = "Stop Param Number %d";
static const char* const REG_KEY_SYSTEM_OUT = "System.out File";
static const char* const REG_KEY_SYSTEM_ERR = "System.err File";
static const char* const REG_KEY_CURRENT_DIR = "Current Directory";
static const char* const REG_KEY_PATH_EXT = "Path";
static const char* const REG_KEY_SHUTDOWN_TIMEOUT = "Shutdown Timeout"; // value added from V1.2.4 release


static const char* const REG_KEY_EVENT_MESSAGE_FILE = "EventMessageFile";
static const char* const REG_KEY_EVENT_TYPES_SUPPORTED = "TypesSupported";




RegistryHandler::RegistryHandler(const char* _serviceName)
: serviceName(_serviceName)
{

}



RegistryHandler::~RegistryHandler()
{
}



bool RegistryHandler::writeServiceParams(const ServiceParameters& serviceParams)
{

	bool written = true; // set to false if any operations fail

	// Create an entry in the registry for this service's parameters.

	const char* serviceKeyName = createServiceKeyName(serviceName);

	HKEY hKey = createRegKey(serviceKeyName);

	deleteKeyName(serviceKeyName); // clean up asap

	if (hKey == NULL)
	{
		written = false; // failed at the first step
	}

	// Store the software version number, for future interrogation if required

	if (written && !storeRegValueString(hKey, REG_KEY_SW_VERSION, STRPRODUCTVER))
	{
		written = false;
	}

	// Set the JVM Library value.

	if (written && !storeRegValueString(hKey, REG_KEY_JVM_LIBRARY, serviceParams.getJvmLibrary()))
	{
		written = false;
	}

	// Set the jvm option count.

	if (written && !storeRegValueDword(hKey, REG_KEY_JVM_OPTION_COUNT, serviceParams.getJvmOptionCount()))
	{
		written = false;
	}

	// Set the jvm options.

	for (int opt = 0; written && (opt < serviceParams.getJvmOptionCount()); opt++)
	{
		char optKeyName[256];
		sprintf(optKeyName, REG_KEY_JVM_OPTION_NO_FMT, opt);

		written = storeRegValueString(hKey, optKeyName, serviceParams.getJvmOption(opt));
	}

	// Set the start class.

	if (written && !storeRegValueString(hKey, REG_KEY_START_CLASS, serviceParams.getStartClass()))
	{
		written = false;
	}

	// Set the start method.

	if (written && !storeRegValueString(hKey, REG_KEY_START_METHOD, serviceParams.getStartMethod()))
	{
		written = false;
	}

	// Set the start parameter count.

	if (written && !storeRegValueDword(hKey, REG_KEY_START_PARAM_COUNT, serviceParams.getStartParamCount()))
	{
		written = false;
	}

	// Set the start parameters.

	for (int param = 0; written && (param < serviceParams.getStartParamCount()); param++)
	{
		char paramKeyName[256];
		sprintf(paramKeyName, REG_KEY_START_PARAM_NO_FMT, param);

		written = storeRegValueString(hKey, paramKeyName, serviceParams.getStartParam(param));
	}

	// Set stop class/method/parameters

	if (written && (serviceParams.getStopClass() != NULL))
	{
		// Set the stop class.

		if (written && !storeRegValueString(hKey, REG_KEY_STOP_CLASS, serviceParams.getStopClass()))
		{
			written = false;
		}

		// Set the stop method.

		if (written && !storeRegValueString(hKey, REG_KEY_STOP_METHOD, serviceParams.getStopMethod()))
		{
			written = false;
		}

		// Set the stop parameter count.

		if (written && !storeRegValueDword(hKey, REG_KEY_STOP_PARAM_COUNT, serviceParams.getStopParamCount()))
		{
			written = false;
		}

		// Set the stop parameters.

		for (int param = 0; written && (param < serviceParams.getStopParamCount()); param++)
		{
			char paramKeyName[256];
			sprintf(paramKeyName, REG_KEY_STOP_PARAM_NO_FMT, param);

			written = storeRegValueString(hKey, paramKeyName, serviceParams.getStopParam(param));
		}
	}

	// Set the out file, if specified

	if (written && !storeRegValueString(hKey, REG_KEY_SYSTEM_OUT, serviceParams.getOutFile()))
	{
		written = false;
	}

	// Set the err file, if specified

	if (written && !storeRegValueString(hKey, REG_KEY_SYSTEM_ERR, serviceParams.getErrFile()))
	{
		written = false;
	}

	// Set the current directory, if specified

	if (written && !storeRegValueString(hKey, REG_KEY_CURRENT_DIR, serviceParams.getCurrentDirectory()))
	{
		written = false;
	}

	// Set the path extension, if specified

	if (written && !storeRegValueString(hKey, REG_KEY_PATH_EXT, serviceParams.getPathExt()))
	{
		written = false;
	}

	// Set the shutdown timeout

	if (written && !storeRegValueDword(hKey, REG_KEY_SHUTDOWN_TIMEOUT, serviceParams.getShutdownMsecs()))
	{
		written = false;
	}

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
	}


	if (written)
	{
		written = setupAnonymousLogging() && setupServiceLogging();
	}

	return written;
}



bool RegistryHandler::readServiceParams(ServiceParameters& serviceParams)
{
	const char* regKeyName = createServiceKeyName(serviceName);

	HKEY hKey = openRegKey(regKeyName);

	deleteKeyName(regKeyName); // clean up asap

	if (hKey == NULL)
	{
		return false; // failed at the first step
	}

	bool read = true; // set to false if any operations fail

	char* tempString = NULL;
	int tempNumber;

	if (getRegValueString(hKey, REG_KEY_SW_VERSION, &tempString))
	{
		serviceParams.setSwVersion(tempString);
		delete[] tempString;
	}
	else
	{
		serviceParams.setSwVersion("V1.x"); // unknown value, but assumed pre-V2 (not an error)
	}

	if (getRegValueString(hKey, REG_KEY_JVM_LIBRARY, &tempString))
	{
		serviceParams.setJvmLibrary(tempString);
		delete[] tempString;
	}
	else
	{
		read = false;
	}

	if (read && getRegValueDword(hKey, REG_KEY_JVM_OPTION_COUNT, &tempNumber))
	{
		serviceParams.setJvmOptionCount(tempNumber);
	}
	else
	{
		read = false;
	}

	if (read && (tempNumber > 0))
	{
		// allocate list of string pointers for all jvm options, then load them
		for (int option = 0; read && (option < tempNumber); option++)
		{
			char optionKeyName[256];
			sprintf(optionKeyName, REG_KEY_JVM_OPTION_NO_FMT, option);

			read = getRegValueString(hKey, optionKeyName, &tempString);
			if (read)
			{
				serviceParams.setJvmOption(option, tempString);
				delete[] tempString;
			}
		}
	}

	if (read && getRegValueString(hKey, REG_KEY_START_CLASS, &tempString))
	{
		serviceParams.setStartClass(tempString);
		delete[] tempString;
	}
	else
	{
		read = false;
	}

	if (read && getRegValueString(hKey, REG_KEY_START_METHOD, &tempString))
	{
		serviceParams.setStartMethod(tempString);
		delete[] tempString;
	}
	else
	{
		read = false;
	}

	if (read && getRegValueDword(hKey, REG_KEY_START_PARAM_COUNT, &tempNumber))
	{
		serviceParams.setStartParamCount(tempNumber);
	}
	else
	{
		read = false;
	}

	if (read && (tempNumber > 0))
	{
		for (int param = 0; read && (param < tempNumber); param++)
		{
			char paramKeyName[256];
			sprintf(paramKeyName, REG_KEY_START_PARAM_NO_FMT, param);

			read = getRegValueString(hKey, paramKeyName, &tempString);
			if (read)
			{
				serviceParams.setStartParam(param, tempString);
				delete[] tempString;
			}
		}
	}

	if (read && getRegValueString(hKey, REG_KEY_STOP_CLASS, &tempString))
	{
		serviceParams.setStopClass(tempString);
		delete[] tempString;
	}
	else
	{
		read = false;
	}

	if (read && getRegValueString(hKey, REG_KEY_STOP_METHOD, &tempString))
	{
		serviceParams.setStopMethod(tempString);
		delete[] tempString;
	}
	else
	{
		read = false;
	}

	if (read && getRegValueDword(hKey, REG_KEY_STOP_PARAM_COUNT, &tempNumber))
	{
		serviceParams.setStopParamCount(tempNumber);
	}
	else
	{
		read = false;
	}

	if (read && (tempNumber > 0))
	{
		for (int param = 0; read && (param < tempNumber); param++)
		{
			char paramKeyName[256];
			sprintf(paramKeyName, REG_KEY_STOP_PARAM_NO_FMT, param);

			read = getRegValueString(hKey, paramKeyName, &tempString);
			if (read)
			{
				serviceParams.setStopParam(param, tempString);
				delete[] tempString;
			}
		}
	}

	// optional parameters may not be defined, so do not flag error if not found in registry

	if (read)
	{
		if (getRegValueString(hKey, REG_KEY_SYSTEM_OUT, &tempString))
		{
			serviceParams.setOutFile(tempString);
			delete[] tempString;
		}
		if (getRegValueString(hKey, REG_KEY_SYSTEM_ERR, &tempString))
		{
			serviceParams.setErrFile(tempString);
			delete[] tempString;
		}
		if (getRegValueString(hKey, REG_KEY_CURRENT_DIR, &tempString))
		{
			serviceParams.setCurrentDirectory(tempString);
			delete[] tempString;
		}
		if (getRegValueString(hKey, REG_KEY_PATH_EXT, &tempString))
		{
			serviceParams.setPathExt(tempString);
			delete[] tempString;
		}

		if (getRegValueDword(hKey, REG_KEY_SHUTDOWN_TIMEOUT, &tempNumber))
		{
			serviceParams.setShutdownMsecs(tempNumber); // else ctor default
		}
	}

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
	}

	return read;
}



bool RegistryHandler::deleteServiceParams()
{

	// attempt deletion of logging-related entries without checking the status

	const char* loggingKeyName = createLoggingKeyName(serviceName);
	deleteRegKey(loggingKeyName);
	deleteKeyName(loggingKeyName); // clean up key name string

	deleteRegKey(ANON_LOGGING_REG_KEY); // note, this might be [have been] shared with multiple instances

	// now delete the service-related entry and return success/failure status

	const char* serviceKeyName = createServiceKeyName(serviceName);
	bool deleted = deleteRegKey(serviceKeyName);
	deleteKeyName(serviceKeyName); // clean up key name string

	return deleted;
}




bool RegistryHandler::setupAnonymousLogging()
{

	// set up legal event source for use before service is actually registered

	return setupLoggingEntries(ANON_LOGGING_REG_KEY);
}


bool RegistryHandler::setupServiceLogging()
{

	// define registry key to support logging messages using the service name.

	const char* loggingKeyName = createLoggingKeyName(serviceName);

	bool setup = setupLoggingEntries(loggingKeyName);
	
	deleteKeyName(loggingKeyName);

	return setup;
}


bool RegistryHandler::setupLoggingEntries(const char* regKeyName)
{
	bool setup = false;
	HKEY hKey = createRegKey(regKeyName);

	if (hKey != NULL)
	{
		// get the [expandable] path location of this executable filename

		char filePath[MAX_PATH];
		GetModuleFileName(NULL, filePath, sizeof(filePath));

		// Store the Event ID message-file name under the 'EventMessageFile' subkey
		// and the flag mask indicating the supported event logging types

		setup = storeRegValueExpString(hKey, REG_KEY_EVENT_MESSAGE_FILE, filePath)
			 && storeRegValueDword(hKey, REG_KEY_EVENT_TYPES_SUPPORTED, EVENT_LOG_TYPES);

		RegCloseKey(hKey);
	}

	return setup;
}



HKEY RegistryHandler::createRegKey(const char* regKeyName)
{
	HKEY hKey = NULL;

	LONG createStatus = RegCreateKeyEx(HKEY_LOCAL_MACHINE, regKeyName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL);
	
	if (createStatus == ERROR_SUCCESS)
	{
		return hKey;
	}
	else
	{
		return NULL;
	}
}


HKEY RegistryHandler::openRegKey(const char* regKeyName)
{
	HKEY hKey = NULL;

	LONG openStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, regKeyName, 0, KEY_QUERY_VALUE, &hKey);
	
	if (openStatus == ERROR_SUCCESS)
	{
		return hKey;
	}
	else
	{
		return NULL;
	}
}


bool RegistryHandler::deleteRegKey(const char* regKeyName)
{

	LONG openStatus = RegDeleteKey(HKEY_LOCAL_MACHINE, regKeyName);
	
	return (openStatus == ERROR_SUCCESS);
}



bool RegistryHandler::storeRegValue(HKEY hRegKey, const char* entryKey, int valueType, BYTE* entryValue, int valueLen)
{
	bool ok = false;

	if (hRegKey == NULL)
	{
		ok = false; // cannot store entry if key handle not available
	}
	else if (entryValue == NULL)
	{
		ok = false; // cannot store entry using a null pointer
	}
	else
	{
		LONG entryStatus = RegSetValueEx(hRegKey, entryKey, 0, valueType,  entryValue, valueLen);
		
		ok = (entryStatus == ERROR_SUCCESS);
	}

	return ok;
}


bool RegistryHandler::storeRegValueString(HKEY hRegKey, const char* entryKey, const char* entryValue)
{

	if (entryValue == NULL)
	{
		return true; // treat as success if no value supplied (does not store anything)
	}
	else
	{
		int valueLen = strlen(entryValue) + 1; // length to include null terminator

		return storeRegValue(hRegKey, entryKey, REG_SZ, (BYTE*)entryValue, valueLen);
	}
}


bool RegistryHandler::storeRegValueExpString(HKEY hRegKey, const char* entryKey, const char* entryValue)
{

	if (entryValue == NULL)
	{
		return false; // cannot treat null expandable-path string as valid
	}
	else
	{
		int valueLen = strlen(entryValue) + 1; // length to include null terminator

		return storeRegValue(hRegKey, entryKey, REG_EXPAND_SZ, (BYTE*)entryValue, valueLen);
	}
}


bool RegistryHandler::storeRegValueDword(HKEY hRegKey, const char* entryKey, const int entryValue)
{

	return storeRegValue(hRegKey, entryKey, REG_DWORD, (BYTE*)&entryValue, sizeof(DWORD));

}


bool RegistryHandler::getRegValueString(HKEY hRegKey, const char* entryKey, char** entryValue)
{
	bool gotValue = false;

	// issue initial registry query to get the data length, and check that it is defined

	DWORD entryLength = 0;
	LONG regStatus = RegQueryValueEx(hRegKey, entryKey, NULL /*lpReserved*/, NULL /*lpType*/, NULL /*lplpData*/, &entryLength);

	if ((regStatus == ERROR_SUCCESS) && (entryLength > 0))
	{
		// allocate buffer to hold the data (must be freed later on)

		*entryValue = (char*)new BYTE[entryLength]; // note, size value returned includes null terminator for strings

		if (entryValue == NULL)
		{
			SetLastError(ERROR_MORE_DATA); // out of memory
		}
		else
		{
			// issue further registry query to get the actual data value

			regStatus = RegQueryValueEx(hRegKey, entryKey, NULL /*lpReserved*/, NULL /*lpType*/, (BYTE*)*entryValue, &entryLength);

			gotValue = (regStatus == ERROR_SUCCESS);
		}
	}

	return gotValue;
}


bool RegistryHandler::getRegValueDword(HKEY hRegKey, const char* entryKey, int* entryValue)
{

	// issue single registry query to get the value (also gets data size, which gets checked too)

	DWORD entryLength = sizeof(DWORD); // need to pass in buffer size, as well as get it back

	LONG regStatus = RegQueryValueEx(hRegKey, entryKey, NULL /*lpReserved*/, NULL /*lpType*/, (BYTE*)entryValue, &entryLength);

	return ((regStatus == ERROR_SUCCESS) && (entryLength > 0));
}



static LONG RegQueryValueExAllocate(HKEY hKey, LPCTSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE *lplpData, LPDWORD lpcbData)
{
	LONG ret;

	//See how long the value is.
	if ((ret=RegQueryValueEx(hKey, lpValueName, lpReserved, lpType, NULL, lpcbData)) != ERROR_SUCCESS)
	{
		return ret;
	}

	//Allocate a buffer for the value.
	*lplpData = new BYTE[*lpcbData];
	if (*lplpData == NULL)
	{
		return ERROR_MORE_DATA;
	}

	//Get the value.
	if ((ret=RegQueryValueEx(hKey, lpValueName, lpReserved, lpType, *lplpData, lpcbData)) != ERROR_SUCCESS)
	{
		return ret;
	}

	return ERROR_SUCCESS;
}



static const char* createServiceKeyName(const char* serviceName)
{
	int keyNameLen = strlen(SERVICE_REG_KEY_PREFIX)
				   + strlen(serviceName)
				   + strlen(SERVICE_REG_KEY_SUFFIX)
				   + 1; // null terminator

	char* keyNameBuff = new char[keyNameLen];
	memset(keyNameBuff, 0, keyNameLen);
	strcpy(keyNameBuff, SERVICE_REG_KEY_PREFIX);
	strcat(keyNameBuff, serviceName);
	strcat(keyNameBuff, SERVICE_REG_KEY_SUFFIX);

	return keyNameBuff;

}

static const char* createLoggingKeyName(const char* serviceName)
{

	int keyNameLen = strlen(SERV_LOGGING_REG_KEY_PREFIX)
				   + strlen(serviceName)
				   + 1; // null terminator

	char* keyNameBuff = new char[keyNameLen];
	memset(keyNameBuff, 0, keyNameLen);
	strcpy(keyNameBuff, SERV_LOGGING_REG_KEY_PREFIX);
	strcat(keyNameBuff, serviceName);

	return keyNameBuff;

}

static void deleteKeyName(const char* regKeyName)
{
	if (regKeyName != NULL)
	{
		delete[] (void*)regKeyName;
	}
}
