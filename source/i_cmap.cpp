//	Zinc Interface Library - I_CMAP.CPP
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


#include "ui_gen.hpp"

#if defined(ZIL_LOAD) && defined(ZIL_DO_FILE_I18N) && defined(ZIL_ISO8859_1)
// Utility to handle errors in the constructor.
ZIL_UINT8 *ZIL_MAP_CHARS::LoadFontMap(ZIL_STORAGE_READ_ONLY &file, const ZIL_ICHAR *name)
{
	ZIL_STORAGE_OBJECT_READ_ONLY *obj =
		new ZIL_STORAGE_OBJECT_READ_ONLY(file, name, 0);
	error = obj->objectError;
	if (error == 0)
	{
		obj->Load(&minVal);
		obj->Load(&maxVal);
		ZIL_UINT8 *tmp = new ZIL_UINT8[maxVal-minVal+1];
		obj->Load(tmp, sizeof(ZIL_UINT8), maxVal-minVal+1);
		delete obj;
		return (tmp);
	}
	delete obj;
	return ZIL_NULLP(ZIL_UINT8);
}
#endif

static ZIL_ICHAR _pathMap[] =
{
	'I','S','O',
	0
};

ZIL_MAP_CHARS::ZIL_MAP_CHARS(const ZIL_ICHAR *_mapName, const ZIL_ICHAR *) :
	doDelete(TRUE),
	fromStandard(ZIL_NULLP(ZIL_UINT8)),
	toStandard(ZIL_NULLP(ZIL_UINT8)),
	error(0)
{
	ZIL_INTERNATIONAL::strcpy(name, _mapName);
#if defined(ZIL_POSIX)
	dirSepStr[0] = '/';
#elif defined(ZIL_MACINTOSH)
	dirSepStr[0] = ':';
#elif defined(ZIL_MSDOS) || defined(ZIL_OS2) || defined(ZIL_MSWINDOWS) || defined(__DVX__)
	dirSepStr[0] = '\\';
#else
	????;
#endif
	dirSepStr[1] = 0;
	if (!_mapName)
		return;
#if defined(ZIL_LOAD) && defined(ZIL_DO_FILE_I18N) && defined(ZIL_ISO8859_1)
	ZIL_STORAGE_READ_ONLY *charMap = ZIL_I18N::defaultStorage;
	if (!charMap)
	{
		charMap = new ZIL_STORAGE_READ_ONLY("iso.dat");
		error = charMap->storageError;
		if (error != 0)
		{
			delete charMap;
			return;
		}
	}
#if defined(ZIL_LOAD)
	ZIL_I18N::Traverse(charMap, _pathMap);
#endif
	if (charMap->ChDir(_mapName) < 0)
	{
		error = charMap->storageError;
		if (charMap != ZIL_I18N::defaultStorage)
			delete charMap;
		return;
	}
	fromStandard = LoadFontMap(*charMap, "toHardware");
	if (!fromStandard)
	{
		error = charMap->storageError;
		if (charMap != ZIL_I18N::defaultStorage)
			delete charMap;
		return;
	}
	toStandard = LoadFontMap(*charMap, "toISO");
	if (!toStandard)
	{
		error = charMap->storageError;
		if (charMap != ZIL_I18N::defaultStorage)
			delete charMap;
		return;
	}
	if (charMap != ZIL_I18N::defaultStorage)
		delete charMap;
#endif
}

ZIL_MAP_CHARS::ZIL_MAP_CHARS(const ZIL_ICHAR *_mapName, const void *_fromStandard, const void *_toStandard, int _doDelete) :
	fromStandard((ZIL_UINT8 *)_fromStandard),
	toStandard((ZIL_UINT8 *)_toStandard),
	minVal(_fromStandard ? 0   : 1),
	maxVal(_fromStandard ? 255 : 0),
	doDelete(_doDelete),
	error(0)
{
	ZIL_INTERNATIONAL::strcpy(name, _mapName);
#if defined(ZIL_POSIX)
	dirSepStr[0] = '/';
#elif defined(ZIL_MACINTOSH)
	dirSepStr[0] = ':';
#elif defined(ZIL_MSDOS) || defined(ZIL_OS2) || defined(ZIL_MSWINDOWS) || defined(__DVX__)
	dirSepStr[0] = '\\';
#else
	????;
#endif
	dirSepStr[1] = 0;
}

ZIL_MAP_CHARS::~ZIL_MAP_CHARS()
{
	if (doDelete)
	{
		delete fromStandard;
		delete toStandard;
	}
}

char *ZIL_MAP_CHARS::MapChar(ZIL_ICHAR mapped)
{
	static char tmpValue[2] = "x";
	tmpValue[0] = mapped;
	if (this == ZIL_NULLP(ZIL_MAP_CHARS))		// PUNT for startup
		return (tmpValue);
	if (fromStandard && minVal <= (ZIL_UINT8)mapped && (ZIL_UINT8)mapped <= maxVal)
		tmpValue[0] = fromStandard[(ZIL_UINT8)mapped];
	return (tmpValue);
}

ZIL_ICHAR ZIL_MAP_CHARS::UnMapChar(const char *unMapped)
{
	if (this == ZIL_NULLP(ZIL_MAP_CHARS))		// PUNT for startup
		return ((ZIL_ICHAR)*unMapped);
	if (toStandard && minVal <= (ZIL_UINT8)*unMapped && (ZIL_UINT8)*unMapped <= maxVal)
		return (toStandard[(ZIL_UINT8)*unMapped]);
	return (*unMapped);
}

char *ZIL_MAP_CHARS::MapText(const ZIL_ICHAR *mapped, char *unMapped, int allocate)
{
	static char ZIL_FARDATA mapTextBuffer[256];
	if (!mapped)
		return(ZIL_NULLP(char));

	char *tmpStr = unMapped;
	int length = sizeof(ZIL_ICHAR)*strlen(mapped)+1;
	if (tmpStr == ZIL_NULLP(char))
	{
		if (allocate)
			tmpStr = new char[length];
		else
		{
			length = sizeof(mapTextBuffer)/sizeof(char);
			tmpStr = mapTextBuffer;
		}
	}
	int i = 0;
	for (; i < length-1 && mapped[i]; i++)
		tmpStr[i] = MapChar(mapped[i])[0];
	tmpStr[i] = 0;
	return (tmpStr);
}

ZIL_ICHAR *ZIL_MAP_CHARS::UnMapText(const char *unMapped, ZIL_ICHAR *mapped, int allocate)
{
	static ZIL_ICHAR ZIL_FARDATA mapTextBuffer[256];
	if (!unMapped)
		return(ZIL_NULLP(ZIL_ICHAR));

	ZIL_ICHAR *tmpStr = mapped;
	int length = sizeof(char)*::strlen(unMapped)+1;
	if (tmpStr == ZIL_NULLP(ZIL_ICHAR))
	{
		if (allocate)
			tmpStr = new ZIL_ICHAR[length];
		else
		{
			length = sizeof(mapTextBuffer)/sizeof(ZIL_ICHAR);
			tmpStr = mapTextBuffer;
		}
	}
	int i = 0;
	for (; i < length-1 && unMapped[i]; i++)
		tmpStr[i] = UnMapChar(&unMapped[i]);
	tmpStr[i] = 0;
	return (tmpStr);
}
