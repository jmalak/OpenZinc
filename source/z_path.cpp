//	Zinc Interface Library - PATH.CPP
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


#if (defined(__ZTC__) && !defined(macintosh)) || defined(_MSC_VER) || (defined(__WATCOMC__) && !defined(__QNX__)) || defined(__HIGHC__) || defined(__IBMCPP__) || defined(__GLOCKENSPIEL__)
#	include <direct.h>
#endif
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#	include <dir.h>
#endif
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
extern "C"
{
#endif
#	include <stdlib.h>
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
}
#endif
#include "ui_gen.hpp"
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
#	include <io.h>
#endif
#if defined(ZIL_POSIX)
extern "C"
{
#	include <unistd.h>
}
#endif

// ----- UI_PATH_ELEMENT & UI_PATH ------------------------------------------

UI_PATH_ELEMENT::UI_PATH_ELEMENT(ZIL_ICHAR *_pathName, int length)
{
	if (length == -1)
		length = strlen(_pathName);
	pathName = new ZIL_ICHAR[length+1];
	strncpy(pathName, _pathName, length);
	pathName[length] = '\0';
}

UI_PATH_ELEMENT::~UI_PATH_ELEMENT(void)
{
	delete pathName;
}

UI_PATH::UI_PATH(ZIL_ICHAR *programPath, int rememberCWD)
{
	// Get the path names.
#if !defined(ZIL_MACINTOSH)
	if (rememberCWD)				// The current working directory.
	{
		ZIL_ICHAR path[256];
		getcwd(path, sizeof(path)/sizeof(path[0]));
		Add(ZIL_NULLP(UI_ELEMENT), new UI_PATH_ELEMENT(path));
	}
#endif

	if (programPath)				// The program path.
	{
		ZIL_ICHAR path[256];
		strcpy(path, programPath);
		ZIL_ICHAR *endPath = strrchr(path, defaultCharMap->dirSepStr[0]);
		if (endPath)
		{
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
			if (endPath > path && *(endPath - 1) == ':')
				endPath++;			// Leave in the slash if root dir.
#endif
			*endPath = '\0';
		}
		Add(ZIL_NULLP(UI_ELEMENT), new UI_PATH_ELEMENT(path));
	}
	LoadPath(_zincPathString);
	LoadPath(_pathString);
}

UI_PATH::~UI_PATH(void)
{
}

#if defined(ZIL_MACINTOSH)
void UI_PATH::LoadPath(const ZIL_ICHAR *)
{
}
#else
void UI_PATH::LoadPath(const ZIL_ICHAR *envname)
{
	ZIL_ICHAR *path;

	if ((path = getenv(envname)) != ZIL_NULLP(ZIL_ICHAR))
	{
		for (int i = 0; path[i]; )	// Parse the directory tree.
		{
			int j;
			for (j = i; path[j] && path[j] != ZIL_PATHSEP; )
				j++;
			Add(ZIL_NULLP(UI_ELEMENT), new UI_PATH_ELEMENT(&path[i], j - i));
			i = path[j] ? j + 1 : j;
		}
	}
}
#endif
