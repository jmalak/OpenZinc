//	Zinc Application Framework - Z_FILE.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

/*       This file is a part of OpenZinc

          OpenZinc is free software; you can redistribute it and/or modify it under
          the terms of the GNU Lessor General Public License as published by
          the Free Software Foundation, either version 3 of the License, or (at
          your option) any later version

	OpenZinc is distributed in the hope that it will be useful, but WITHOUT
          ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
          or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser 
          General Public License for more details.

          You should have received a copy of the GNU Lessor General Public License
	 along with OpenZinc. If not, see <http://www.gnu.org/licenses/>                          */


#include "ui_gen.hpp"
#if defined(__MWERKS__)
#	include <unix.h>
#else
#	include <fcntl.h>
#endif
#if defined(ZIL_MACINTOSH)
#	define S_IREAD 0
#	define S_IWRITE 0
#	if !defined(O_TEXT)
#		define O_TEXT 0
#	endif
#else
#	include <sys/stat.h>
#	if defined(ZIL_POSIX)
extern "C"
{
#		include <unistd.h>
#		if defined(ZIL_NEXTSTEP)
#			include <libc.h>
#		endif
}
#		if !defined(O_BINARY)
#			define O_BINARY 0
#		endif
#		if !defined(O_TEXT)
#			define O_TEXT 0
#		endif
#	else
#		include <io.h>
#		if defined (__IBMCPP__) || defined (_MSC_VER)
#			include <stdio.h>
#		endif
#	endif
#endif

// ----- General Functions --------------------------------------------------

ZIL_FILE::ZIL_FILE(const ZIL_ICHAR *_pathName, UIS_FLAGS _access) :
	access(_access), error(ERROR_NONE)
{
	// Set the class members.
	handle = -1;
	pathName = strdup(_pathName);

	// Map to actual access flags.
#if defined(ZIL_POSIX)
	osMode = 0664;
#else
	osMode = S_IREAD;
	if (FlagSet(_access, UIS_READWRITE))
		osMode |= S_IWRITE;
#endif
	osAccess = 0;
	if (FlagSet(_access, UIS_BINARY))
		osAccess |= O_BINARY;
	else if (FlagSet(_access, UIS_TEXT))
		osAccess |= O_TEXT;
	if (FlagSet(_access, UIS_READ))
		osAccess |= O_RDONLY;
	else if (FlagSet(_access, UIS_READWRITE))
		osAccess |= O_RDWR;
	if (FlagSet(_access, UIS_CREATE))
		osAccess |= O_CREAT | O_TRUNC;
	else if (FlagSet(_access, UIS_OPENCREATE))
		osAccess |= O_CREAT;

	// Open the file.
	Open();
}

ZIL_FILE::~ZIL_FILE(void)
{
	// Close the file and deallocate the name.
	Close();
	if (pathName)
		delete pathName;
}

ZIL_FILE::Z_ERROR ZIL_FILE::Close(void)
{
#if defined(ZIL_MACINTOSH)
	if (handle != -1)
		FSClose(handle);
#else
	if (handle != -1)
		::close(handle);
#endif
	handle = -1;
	return (ERROR_NONE);
}

ZIL_FILE::Z_ERROR ZIL_FILE::GetError(void)
{
	return (error);
}

long ZIL_FILE::Length(void) const
{
#if defined(ZIL_MACINTOSH)
	long position = 0L;
	GetEOF(handle, &position);
#else
	long current = ::lseek(handle, 0, SEEK_CUR);
	long position = ::lseek(handle, 0, SEEK_END);
	::lseek(handle, current, SEEK_SET);
#endif
	return (position);
}

ZIL_FILE::Z_ERROR ZIL_FILE::Open(void)
{
	if (handle != -1)
		return (ERROR_ACCESS);

	handle = ZIL_INTERNATIONAL::open(pathName, osAccess, osMode);

	if (handle < 0)
		return (ERROR_ACCESS);
	else
		return (ERROR_NONE);
}

ZIL_FILE::Z_ERROR ZIL_FILE::Rename(const ZIL_ICHAR *_pathName)
{
	// Rename the file.
	rename(pathName, _pathName);
	delete pathName;
	pathName = strdup(_pathName);

	return (ERROR_NONE);
}

ZIL_FILE::Z_ERROR ZIL_FILE::Seek(long offset, SEEK location) const
{
#if defined(ZIL_MACINTOSH)
	short fromLoc = fsFromStart;
	switch (location)
	{
	case SEEK_FROM_START:
		fromLoc = fsFromStart;
		break;
	case SEEK_FROM_CURRENT:
		fromLoc = fsFromMark;
		break;
	case SEEK_FROM_END:
		fromLoc = fsFromLEOF;
		break;
	}
	SetFPos(handle, fromLoc, offset);
#else
	::lseek(handle, offset, location);
#endif
	return (ERROR_NONE);
}

ZIL_FILE::Z_ERROR ZIL_FILE::SetError(Z_ERROR _error)
{
	error = _error;
	return error;
}

long ZIL_FILE::Tell(void) const
{
	long position;
#if defined(ZIL_MACINTOSH)
	GetFPos(handle, &position);
#else
	position = ::lseek(handle, 0, SEEK_CUR);
#endif
	return (position);
}

ZIL_FILE::Z_ERROR ZIL_FILE::Unlink(void)
{
	handle = ZIL_INTERNATIONAL::unlink(pathName);

	return (ERROR_NONE);
}

// ----- Read Functions -----------------------------------------------------

int ZIL_FILE::Read(ZIL_INT8 *value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(*value);
	if (FSRead(handle, &count, (Ptr)value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (read(handle, value, sizeof(*value)));
#endif
}

int ZIL_FILE::Read(ZIL_UINT8 *value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(*value);
	if (FSRead(handle, &count, (Ptr)value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (read(handle, (char *)value, sizeof(*value)));
#endif
}

int ZIL_FILE::Read(ZIL_INT16 *value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(*value);
	if (FSRead(handle, &count, (Ptr)value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (read(handle, (char *)value, sizeof(*value)));
#endif
}

int ZIL_FILE::Read(ZIL_UINT16 *value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(*value);
	if (FSRead(handle, &count, (Ptr)value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (read(handle, (char *)value, sizeof(*value)));
#endif
}

int ZIL_FILE::Read(ZIL_INT32 *value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(*value);
	if (FSRead(handle, &count, (Ptr)value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (read(handle, (char *)value, sizeof(*value)));
#endif
}

int ZIL_FILE::Read(ZIL_UINT32 *value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(*value);
	if (FSRead(handle, &count, (Ptr)value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (read(handle, (char *)value, sizeof(*value)));
#endif
}

int ZIL_FILE::Read(ZIL_ICHAR *text, int length) const
{
	// Note: The argument length is the number of ZIL_ICHARs to read,
	// whereas the return int is the number of bytes read.
	if (length == -1)
	{
		ZIL_INT16 value; Read(&value); length = value;
		text[length] = '\0';
	}
#	if defined(ZIL_UNICODE)
	char *isoText = new char[2*length+1];
	int bytesRead = read(handle, isoText, 2*length);
	mbstowcs(text, isoText, length);
	delete isoText;
	return (bytesRead);
#	elif defined(ZIL_ISO8859_1)
#		if defined(ZIL_MACINTOSH)
		ZIL_ICHAR *unmappedText;
		long count = length;
		OSErr theError = FSRead(handle, &count, text);
		unmappedText = UnMapText(text);
		strcpy(text, unmappedText);
		if (theError == noErr || theError == eofErr)
			return ((int)count);
		else
			return (-1);
#		else
		int count = read(handle, text, length);
		ZIL_ICHAR *unmappedText;
		unmappedText = UnMapText(text);
		strcpy(text, unmappedText);
		return ((int)count);
#		endif
#	else
#		if defined(ZIL_MACINTOSH)
	long count = length;
	OSErr theError = FSRead(handle, &count, text);
	if (theError == noErr || theError == eofErr)
		return ((int)count);
	else
		return (-1);
#		else
	return(read(handle, text, length));
#		endif
#	endif
}

int ZIL_FILE::Read(ZIL_ICHAR **text) const
{
	ZIL_INT16 value; Read(&value); int length = value;
	*text = new ZIL_ICHAR[length+1];
	*text[length] = '\0';
	return (Read(*text, length));
}

int ZIL_FILE::Read(void *buffer, int size, int length) const
{
#if defined(ZIL_MACINTOSH)
	long count = size * length;
	OSErr theError = FSRead(handle, &count, (Ptr)buffer);
	if (theError == noErr || theError == eofErr)
		return ((int)count);
	else
		return (-1);
#else
	return (read(handle, (char *)buffer, size * length));
#endif
}

// ----- Write Functions ----------------------------------------------------

int ZIL_FILE::Write(ZIL_INT8 value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(value);
	if (FSWrite(handle, &count, (Ptr)&value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (write(handle, &value, sizeof(value)));
#endif
}

int ZIL_FILE::Write(ZIL_UINT8 value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(value);
	if (FSWrite(handle, &count, (Ptr)&value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (write(handle, (char *)&value, sizeof(value)));
#endif
}

int ZIL_FILE::Write(ZIL_INT16 value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(value);
	if (FSWrite(handle, &count, (Ptr)&value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (write(handle, (char *)&value, sizeof(value)));
#endif
}

int ZIL_FILE::Write(ZIL_UINT16 value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(value);
	if (FSWrite(handle, &count, (Ptr)&value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (write(handle, (char *)&value, sizeof(value)));
#endif
}

int ZIL_FILE::Write(ZIL_INT32 value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(value);
	if (FSWrite(handle, &count, (Ptr)&value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (write(handle, (char *)&value, sizeof(value)));
#endif
}

int ZIL_FILE::Write(ZIL_UINT32 value) const
{
#if defined(ZIL_MACINTOSH)
	long count = sizeof(value);
	if (FSWrite(handle, &count, (Ptr)&value) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (write(handle, (char *)&value, sizeof(value)));
#endif
}

int ZIL_FILE::Write(ZIL_ICHAR *text) const
{
	int length = strlen(text);
#	if defined(ZIL_UNICODE)
	length = length * sizeof(ZIL_ICHAR) + 1;
	char *isoText = new char[length];
	wcstombs(isoText, text, length);
	for (length = 0; isoText[length]; length++)
		;
	length = write(handle, isoText, length);
	delete isoText;
	return (length);
#	elif defined(ZIL_ISO8859_1)
#		if defined(ZIL_MACINTOSH)
	long count = length * sizeof(ZIL_ICHAR);
	char *mappedText;
	mappedText = MapText(text);
	strcpy(text, mappedText);
	if (FSWrite(handle, &count, text) == noErr)
		return ((int)count);
	else
		return (-1);
#		else
	char *mappedText;
	mappedText = MapText(text);
	strcpy(text, mappedText);
	int written = write(handle, text, length * sizeof(ZIL_ICHAR));
	return (written);
#		endif
#	else
#		if defined(ZIL_MACINTOSH)
	long count = length * sizeof(ZIL_ICHAR);
	if (FSWrite(handle, &count, text) == noErr)
		return ((int)count);
	else
		return (-1);
#		else
	return (write(handle, text, length * sizeof(ZIL_ICHAR)));
#		endif
#	endif
}

int ZIL_FILE::Write(void *buffer, int size, int length) const
{
#if defined(ZIL_MACINTOSH)
	long count = size * length;
	if (FSWrite(handle, &count, (Ptr)buffer) == noErr)
		return ((int)count);
	else
		return (-1);
#else
	return (write(handle, (char *)buffer, size * length));
#endif
}

// ----- Special UNICODE file functions -------------------------------------

#if defined(ZIL_UNICODE)
int ZIL_FILE::Read(char *text, int length) const
{
	if (length == -1)
	{
		ZIL_INT16 value; Read(&value); length = value;
		text[length] = '\0';
	}
	return (read(handle, text, length));
}

int ZIL_FILE::Read(char **text) const
{
	ZIL_INT16 value; Read(&value); int length = value;
	*text = new char[length+1];
	*text[length] = '\0';
	return (Read(*text, length));
}

int ZIL_FILE::Write(char *text) const
{
	int length = ::strlen(text);
	return (write(handle, text, length));
}
#endif
