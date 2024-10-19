//	Zinc Interface Library - I_CFILE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
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


#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
}
#endif
#include "ui_env.hpp"
#if defined(_MSC_VER)
#	include<sys/types.h>
#endif
#if defined(ZIL_MACINTOSH)
#	include <Aliases.h>
#	include <errno.h>
#	if defined(__MWERKS__)
#		include <unix.h>	// For O_CREAT.
#	else
#		include <fcntl.h>	// For O_CREAT.
#	endif
#else
extern "C"
{
#	include <sys/stat.h>
}
#	if defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
#		include <dir.h>
#	elif defined(__ZTC__) || defined(_MSC_VER) || (defined(__WATCOMC__) && !defined(__QNX__)) || defined(__HIGHC__) || defined(__IBMCPP__) || defined(__GLOCKENSPIEL__)
#		include <direct.h>
#	endif
#endif
#if defined(ZIL_POSIX)
extern "C"
{
#if defined(ZIL_NEXTSTEP)
#	include <libc.h>
#endif
#	include <fcntl.h>
#	include <unistd.h>
}
#elif !defined(ZIL_MACINTOSH)
#	include <io.h>
#endif
#include "ui_gen.hpp"

#if defined(ZIL_MACINTOSH)
int ZIL_INTERNATIONAL::chdir(const ZIL_ICHAR *)
	{
		return (-1);
	}
#else
int ZIL_INTERNATIONAL::chdir(const ZIL_ICHAR *pathName)
	{
		return (::chdir((char *)pathName)); // Typecast for IBM compiler.
	}
#endif

#if defined(ZIL_MACINTOSH) || defined(ZIL_NEXTSTEP)
ZIL_ICHAR *ZIL_INTERNATIONAL::getcwd(ZIL_ICHAR *, unsigned)
	{
		return (ZIL_NULLP(ZIL_ICHAR));
	}
#else
ZIL_ICHAR *ZIL_INTERNATIONAL::getcwd(ZIL_ICHAR *buffer, unsigned length)
	{
		return (::getcwd(buffer, length));
	}
#endif

#if defined(ZIL_MACINTOSH)
ZIL_ICHAR *ZIL_INTERNATIONAL::getenv(const ZIL_ICHAR *)
	{
		return (ZIL_NULLP(ZIL_ICHAR));
	}
#else
ZIL_ICHAR *ZIL_INTERNATIONAL::getenv(const ZIL_ICHAR *name)
	{
		return (::getenv(name));
	}
#endif

#if defined(ZIL_MACINTOSH)
int ZIL_INTERNATIONAL::open(const ZIL_ICHAR *name, int oflag, unsigned)
	{
		Str255 fName;
		MapText(name, (char *)fName, FALSE);
		c2pstr((char *)fName);
		if (oflag & O_CREAT)
		{
			errno = Create(fName, 0, fileCreator, fileType);
			if (errno != noErr && errno != dupFNErr)
				return (-1);
		}
		else
		{
			// We must resolve, if this is an alias.
			Boolean isFolder, isAlias;
			FSSpec theSpec;
			if (FSMakeFSSpec(0, 0L, fName, &theSpec) == noErr &&
				ResolveAliasFile(&theSpec, TRUE, &isFolder, &isAlias) == noErr &&
				isAlias)
			{
				GetPathNameFromDirID(fName, theSpec.parID, theSpec.vRefNum);
				pstrcat(fName, theSpec.name);
			}
		}
		SignedByte permission;
		short refNum;
#	if defined(__ZTC__)
		// Because Symantec says O_RDONLY is 0.
		if (!FlagSet(oflag, (O_WRONLY | O_RDWR)))
#	else
		if (FlagSet(oflag, O_RDONLY))
#	endif
			permission = fsRdPerm;
		else
			permission = fsRdWrPerm;

		errno = HOpenDF(0, 0L, fName, permission, &refNum);
		if (errno == noErr)
			return (refNum);
		else
		{
			if (oflag & O_CREAT)
				FSDelete(fName, 0);
			return (-1);
		}
	}
#else
int ZIL_INTERNATIONAL::open(const ZIL_ICHAR *name, int oflag, unsigned mode)
	{
		return (::open(name, oflag, mode));
	}
#endif

int ZIL_INTERNATIONAL::rename(const ZIL_ICHAR *oldName, const ZIL_ICHAR *newName)
	{
#if defined(ZIL_MACINTOSH)
		Str255 fName, tName;
		MapText(oldName, (char *)fName, FALSE);
		MapText(newName, (char *)tName, FALSE);
		c2pstr((char *)fName);
		c2pstr((char *)tName);
		errno = Rename(fName, 0, tName);
		if (errno == noErr)
			return (0);
		else
			return (-1);
#else
		return (::rename(oldName, newName));
#endif
	}

#if defined(ZIL_MACINTOSH)
int ZIL_INTERNATIONAL::stat(const ZIL_ICHAR *path, void *)
	{
		char tmp[255];
		MapText(path, tmp, FALSE);
		FILE *fp = fopen(tmp, "r");
		if (fp == ZIL_NULLP(FILE))
			return (-1);

		fclose(fp);
		return (0);
	}
#elif defined(ZIL_NEXTSTEP) && defined(__NX_COMPILER_RELEASE_3_1)
int ZIL_INTERNATIONAL::stat(const ZIL_ICHAR *path, void *info)
	{
		int (*sptr)(const char *, struct stat *) = ::stat;
		return (sptr(path, info));
	}
#else
typedef struct stat Z_STAT; // Explicit typedef for Zortech/Symantec bug.
int ZIL_INTERNATIONAL::stat(const ZIL_ICHAR *path, void *info)
	{
		return (::stat(path, (Z_STAT *)info));
	}
#endif

ZIL_ICHAR *ZIL_INTERNATIONAL::tmpnam(ZIL_ICHAR *name)
	{ return (::tmpnam(name)); }

int ZIL_INTERNATIONAL::unlink(const ZIL_ICHAR *pathName)
	{
#if defined(ZIL_MACINTOSH)
		Str255 fName;
		MapText(pathName, (char *)fName, FALSE);
		c2pstr((char *)fName);
		errno = FSDelete(fName, 0);
		if (errno == noErr)
			return (0);
		else
			return (-1);
#elif defined(__SC__)
		return (::remove((ZIL_ICHAR *)pathName));
#else
		return (::unlink(pathName));
#endif
	}
