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

#ifndef __EVENT_LOGGER_H__
#define __EVENT_LOGGER_H__

#include <windows.h>


HANDLE registerDefaultEventSource();

HANDLE registerServiceEventSource(const char* serviceName);

void deregisterEventSource(HANDLE* hEventSource);

void logMessage(HANDLE hEventSource, const char* logText);

void logError(HANDLE hEventSource, const char* errText);

void logEventMessage(HANDLE hEventSource, const char* messageText, int messageType);

void logFunctionError(HANDLE hEventSource, const char* functionName);

void logFunctionMessage(HANDLE hEventSource, const char* functionName, const char* messageText);

#endif // __EVENT_LOGGER_H__
