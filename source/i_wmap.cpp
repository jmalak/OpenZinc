//	Zinc Interface Library - I_WMAP.CPP
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


#include "ui_evt.hpp"

// Load maps into memory for faster lookups.
//#define ZIL_FAST_MAP
//#define ZIL_SEEK

// This package contains the code necessary to map from Unicode to the
// operating system specific character mapping.  This mapping may be
// BIG5, Shift JIS, or something else (even Unicode).  Since any paticular
// platform may only support a subset of the Unicode characters, this
// mapping is done by creating "spans" of characters that map back and
// forth.  In other words, we implement the two mapping functions as two
// sparse vectors, one for each direction.
// In reality, we load up two maps for each direction (a total of four maps).
// The larger ones are the major maps (BIG5, Shift JIS, etc.).  The smaller
// ones (the "Extra"s) are for the specific hardware/OS combination.  This
// allows us to share most of the map between related but different platforms
// (i.e. DOS/V, NEC PC9800, MS-Windows all use Shift JIS, but have different
//  0x00 - 0x7f characters).
#if defined(ZIL_MSDOS) || defined(ZIL_OS2) || defined(ZIL_MSWINDOWS)
#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))
static struct
{
	ZIL_ICHAR name[8];
	ZIL_ICHAR dirSep;
} ZIL_FARDATA specialDirSeps[] =
{
	{ { 'I','B','M','_','9','3','2',0 }, 0xa5 },
	{ { 'I','B','M','_','9','4','2',0 }, 0xa5 },
	{ { 'I','B','M','_','9','4','9',0 }, 0x20a9 }
};
#endif

const ZIL_ICHAR defaultHardware = 127;
const ZIL_ICHAR defaultUnicode  = 0xFFFD;
static ZIL_ICHAR ZIL_FARDATA _mapDirName[] = { 'M','a','p','p','i','n','g',0 };
static ZIL_ICHAR ZIL_FARDATA _toHardwareName[] = { 't','o','H','a','r','d','w','a','r','e',0 };
static ZIL_ICHAR ZIL_FARDATA _toUnicodeName[] = { 't','o','U','n','i','c','o','d','e',0 };

typedef struct
{
	ZIL_UINT16 minChar;		// MinValue char in this span
	ZIL_UINT16 maxChar;		// MaxValue char in this span
	long position;		// Seek position in object of this span
#if !defined(ZIL_FAST_MAP)
	ZIL_UINT16 *map;
#endif
} spanEntry;

class ZIL_EXPORT_CLASS ZIL_CHAR_MAP
{
	ZIL_UINT16 numSpans;	// Number of spans in array
	spanEntry *spanArray;	// Span array
	ZIL_STORAGE_OBJECT_READ_ONLY *map;	// Object that contains the map
#if defined(ZIL_FAST_MAP)
	ZIL_ICHAR *mapArray[256];
#endif
public:
	int error;

	ZIL_CHAR_MAP(ZIL_STORAGE_READ_ONLY &file, const ZIL_ICHAR *name);
	~ZIL_CHAR_MAP();
	ZIL_ICHAR DoMap(ZIL_ICHAR val);

	ZIL_ICHAR defaultChar;
};

// In the file, open the object "name" that contains a translation map.
ZIL_CHAR_MAP::ZIL_CHAR_MAP(ZIL_STORAGE_READ_ONLY &file, const ZIL_ICHAR *name) :
	error(0)
{
	map = new ZIL_STORAGE_OBJECT_READ_ONLY(file, name, 0);
	if ((error = map->objectError) != 0)
	{
		delete map;
		map = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY);
		return;
	}
	ZIL_UINT16 c;
	map->Load(&c);
	defaultChar = ' ';
	map->Load(&numSpans);
	spanArray = new spanEntry[numSpans];
	long position = map->Tell() + numSpans * sizeof(ZIL_UINT16) * 2;
	for (int i=0; i < numSpans; i++)
	{
		map->Load(&spanArray[i].minChar);
		map->Load(&spanArray[i].maxChar);
		spanArray[i].position = position;
		position += (spanArray[i].maxChar - spanArray[i].minChar + 1) * sizeof(ZIL_ICHAR);
#if !defined(ZIL_FAST_MAP)
		spanArray[i].map = ZIL_NULLP(ZIL_UINT16);
#endif
	}
#if defined(ZIL_FAST_MAP)
	for (i=0; i < 256; i++)
		mapArray[i] = ZIL_NULLP(ZIL_ICHAR);
#endif
}

ZIL_CHAR_MAP::~ZIL_CHAR_MAP()
{
	if (error == 0)
	{
		delete map;
#if defined(ZIL_FAST_MAP)
		for (int i=0; i < 256; i++)
			delete mapArray[i];
#else
		for (int i=0; i < numSpans; i++)
			delete spanArray[i].map;
#endif
		delete []spanArray;
	}
}

// Map from a character in one mapping to a character in another.
ZIL_ICHAR ZIL_CHAR_MAP::DoMap(ZIL_ICHAR val)
{
#if defined(ZIL_FAST_MAP)
	if (val == 0)
		return (0);
	ZIL_ICHAR *line = mapArray[(val >> 8) & 0xff];
	if (line && line[val & 0xff])
		return (line[val & 0xff]);
#endif
	if (val < spanArray[0].minChar ||
	    val > spanArray[numSpans-1].maxChar)
		return (defaultChar);
	// Binary search the spans.
	int i = 0;
	int j = numSpans-1;
	while (val >= spanArray[i].minChar &&
	       val <= spanArray[j].maxChar)
	{
		int k = (i + j) / 2;
		if (val < spanArray[k].minChar)
			j = k - 1;
		else if (val > spanArray[k].maxChar)
			i = k + 1;
		else
		{
#if defined(ZIL_SEEK)
			map->Seek(spanArray[k].position + sizeof(ZIL_ICHAR) * (val - spanArray[k].minChar));
			map->Load(&val);
			return (val);
#elif defined(ZIL_FAST_MAP)
			map->Seek(spanArray[k].position);
			for (ZIL_ICHAR i=spanArray[k].minChar; i <= spanArray[k].maxChar; i++)
			{
				line = mapArray[(i >> 8) & 0xff];
				if (!line)
				{
					line = mapArray[(i >> 8) & 0xff] = new ZIL_ICHAR[256];
					for (int j=0; j < 256; j++)
						line[j] = 0;
				}
				ZIL_UINT16 tmp;
				map->Load(&tmp);
				line[i & 0xff] = tmp;
			}
			line = mapArray[(val >> 8) & 0xff];
			return (line[val & 0xff]);
#else
			if (!spanArray[k].map)
			{
				map->Seek(spanArray[k].position);
				spanArray[k].map = new ZIL_UINT16[spanArray[k].maxChar-spanArray[k].minChar+1];
				for (int i=0; i <= spanArray[k].maxChar-spanArray[k].minChar; i++)
					map->Load(&spanArray[k].map[i]);
			}
			return (spanArray[k].map[val - spanArray[k].minChar]);
#endif
		}
	}
#if defined(ZIL_FAST_MAP)
#	if !defined(ZIL_MACINTOSH)
	// This would cause a Macintosh to hang for some reason.
	if (!line)
	{
		line = mapArray[(i >> 8) & 0xff] = new ZIL_ICHAR[256];
		for (i=0; i < 256; i++)
			line[i] = 0;
	}
#	endif
	if (line)
		line[val & 0xff] = defaultChar;
#endif
	return (defaultChar);
}

// Utility to handle errors in the constructor.
ZIL_CHAR_MAP *ZIL_MAP_CHARS::LoadFontMap(ZIL_STORAGE_READ_ONLY &file, const ZIL_ICHAR *name)
{
	ZIL_CHAR_MAP *tmp;

	tmp = new ZIL_CHAR_MAP(file, name);
	if (tmp->error == 0)
		return (tmp);
	delete tmp;
	return ZIL_NULLP(ZIL_CHAR_MAP);
}

static ZIL_ICHAR ZIL_FARDATA _pathMap[] =
{
	'U','N','I','C','O','D','E',
	0
};

ZIL_MAP_CHARS::ZIL_MAP_CHARS(const ZIL_ICHAR *_mapName, const ZIL_ICHAR *extraName) :
	doDelete(TRUE),
	mapUnicodeToLocal(ZIL_NULLP(ZIL_ICHAR)),
	toHardwareDefault(ZIL_NULLP(ZIL_CHAR_MAP)),
	toUnicodeDefault(ZIL_NULLP(ZIL_CHAR_MAP)),
	toHardwareExtra(ZIL_NULLP(ZIL_CHAR_MAP)),
	toUnicodeExtra(ZIL_NULLP(ZIL_CHAR_MAP)),
	error(0)
{
	ZIL_INTERNATIONAL::strcpy(name, _mapName);
#if defined(ZIL_POSIX)
	dirSepStr[0] = '/';
#elif defined(ZIL_MACINTOSH)
	dirSepStr[0] = ':';
#elif defined(ZIL_MSDOS) || defined(ZIL_OS2) || defined(ZIL_MSWINDOWS)
	dirSepStr[0] = '\\';
	for (int j=0; j < LENGTHOF(specialDirSeps); j++)
		if (ZIL_INTERNATIONAL::streq(_mapName, specialDirSeps[j].name) == 0)
		{
			dirSepStr[0] = specialDirSeps[j].dirSep;
			break;
		}
#else
	????;
#endif
	dirSepStr[1] = 0;

	if (!_mapName)
		return;
	if (!ZIL_I18N::defaultStorage)
	{
		ZIL_ICHAR *mapFileName = ZIL_INTERNATIONAL::ISOtoUNICODE("unicode.dat");
		charMap = new ZIL_STORAGE_READ_ONLY(mapFileName);
		delete mapFileName;
		error = charMap->storageError;
		if (error != 0)
			return;
	}
	else
		charMap = ZIL_I18N::defaultStorage;
#if defined(ZIL_LOAD)
	ZIL_I18N::Traverse(charMap, _pathMap);
#endif
	if (charMap->ChDir(_mapName) < 0)
	{
		error = charMap->storageError;
		return;
	}
	toHardwareDefault = LoadFontMap(*charMap, _toHardwareName);
	if (!toHardwareDefault)
	{
		error = charMap->storageError;
		return;
	}
	toUnicodeDefault = LoadFontMap(*charMap, _toUnicodeName);
	if (!toUnicodeDefault)
	{
		error = charMap->storageError;
		return;
	}

	// Newer compilers need these braces, since the constructor call for map
	// is considered an initializer that cannot be skipped by the returns above.
	{
		ZIL_STORAGE_OBJECT_READ_ONLY map(*charMap, _mapDirName, 0);
		if (map.objectError != 0)
		{
			error = charMap->storageError = map.objectError;
			return;
		}
		map.Load(&leadinSize);
		int max = leadinSize;
		if (max < 0)
			max = 1;
		for (int i=0; i < max; i++)
		{
			map.Load(&leadins[i].min);
			map.Load(&leadins[i].max);
		}
	}

	if (!extraName)
		return;
	if (charMap->ChDir(extraName) < 0)
	{
		error = charMap->storageError;
		return;
	}
	toHardwareExtra = LoadFontMap(*charMap, _toHardwareName);
	if (!toHardwareExtra)
	{
		error = charMap->storageError;
		return;
	}
	toUnicodeExtra = LoadFontMap(*charMap, _toUnicodeName);
	if (!toUnicodeExtra)
	{
		error = charMap->storageError;
		return;
	}
}

ZIL_MAP_CHARS::ZIL_MAP_CHARS(const ZIL_ICHAR *_mapName, const void *_fromStandard, const void *, int _doDelete) :
	leadinSize(0),
	toHardwareDefault(ZIL_NULLP(ZIL_CHAR_MAP)),
	toUnicodeDefault(ZIL_NULLP(ZIL_CHAR_MAP)),
	toHardwareExtra(ZIL_NULLP(ZIL_CHAR_MAP)),
	toUnicodeExtra(ZIL_NULLP(ZIL_CHAR_MAP)),
	charMap(ZIL_NULLP(ZIL_STORAGE_READ_ONLY)),
	mapUnicodeToLocal((ZIL_ICHAR *)_fromStandard),
	doDelete(_doDelete),
	error(0)
{
	ZIL_INTERNATIONAL::strcpy(name, _mapName);
#if defined(ZIL_POSIX)
	dirSepStr[0] = '/';
#elif defined(ZIL_MACINTOSH)
	dirSepStr[0] = ':';
#elif defined(ZIL_MSDOS) || defined(ZIL_OS2) || defined(ZIL_MSWINDOWS)
	dirSepStr[0] = '\\';
	for (int j=0; j < LENGTHOF(specialDirSeps); j++)
		if (ZIL_INTERNATIONAL::streq(_mapName, specialDirSeps[j].name) == 0)
		{
			dirSepStr[0] = specialDirSeps[j].dirSep;
			break;
		}
#else
	????;
#endif
	dirSepStr[1] = 0;
}

ZIL_MAP_CHARS::~ZIL_MAP_CHARS()
{
	if (doDelete)
	{
		delete toHardwareDefault;
		delete toUnicodeDefault;
		delete toHardwareExtra;
		delete toUnicodeExtra;
		if (charMap != ZIL_I18N::defaultStorage)
			delete charMap;
	}
}

char *ZIL_MAP_CHARS::MapChar(ZIL_ICHAR mapped)
{
	static char unMapped[4];
	unMapped[0] = mapped;
	unMapped[1] = 0;
	if (this == ZIL_NULLP(ZIL_MAP_CHARS))		// PUNT for startup
		return (unMapped);

	if (!toHardwareExtra && !toHardwareDefault)
	{
#if defined(ZIL_MSWINDOWS) || defined(ZIL_MOTIF)
		// these are ISO
		if (mapped < 256)
			return (unMapped);
#else
		// Dos TEXT, OS/2 and Macintosh
		if (!mapUnicodeToLocal)
			return (unMapped);
		for (int i=0; i < 256; i++)
			if (mapUnicodeToLocal[i] == mapped)
			{
				unMapped[0] = i;
				return (unMapped);
			}
#endif
	}
	// Assume everyone else can at least do 7bit ASCII
	ZIL_ICHAR tmpValue;
	if (toHardwareExtra &&
	    (tmpValue = toHardwareExtra->DoMap(mapped))
	    != toHardwareExtra->defaultChar)
		mapped = tmpValue;
	else if (toHardwareDefault)
	{
		tmpValue = toHardwareDefault->DoMap(mapped);
#if defined(ZIL_MSDOS) || defined(ZIL_OS2) || defined(ZIL_MSWINDOWS)
		if (mapped == '\\' && tmpValue == toHardwareDefault->defaultChar)
		{
			for (int i=0; i < LENGTHOF(specialDirSeps); i++)
				if (ZIL_INTERNATIONAL::streq(name, specialDirSeps[i].name) == 0)
				{
					tmpValue = '\\';
					break;
				}
		}
#endif
		mapped = tmpValue;
	}
	else
		mapped = defaultHardware;
	unMapped[0] = mapped & 0xFF;
	if (mapped >= 256)
	{
		unMapped[0] = (mapped >> 8) & 0xff;
		unMapped[1] = mapped & 0xFF;
		unMapped[2] = 0;
	}
	return (unMapped);
}

ZIL_ICHAR ZIL_MAP_CHARS::UnMapChar(const char *unMapped)
{
	if (this == ZIL_NULLP(ZIL_MAP_CHARS))		// PUNT for startup
		return ((ZIL_ICHAR)*unMapped);

	if (!toUnicodeExtra && !toUnicodeDefault)
	{
#if defined(ZIL_MSWINDOWS) || defined(ZIL_MOTIF)
		// these are ISO
		return ((ZIL_ICHAR)*unMapped);
#else
		// Dos TEXT, OS/2 and Macintosh
		if (!mapUnicodeToLocal)
			return (*unMapped & 0xFF);
		if ((*unMapped & 0xFF) < 256)
			return (mapUnicodeToLocal[*unMapped & 0xFF]);
#endif
	}
	int j = mblen(unMapped);
	ZIL_ICHAR tmpValue = 0;
	for (int i = 0; i < j; i++)
		tmpValue |= ((ZIL_UINT8)unMapped[i]) << 8*(j-i-1);
	ZIL_ICHAR mapped = defaultUnicode;
	if (toUnicodeDefault &&
	    (mapped = toUnicodeDefault->DoMap(tmpValue))
	    != toUnicodeDefault->defaultChar)
		return (mapped);
	if (toUnicodeExtra &&
	    (mapped = toUnicodeExtra->DoMap(tmpValue))
	    != toUnicodeExtra->defaultChar)
		return (mapped);
	return (mapped);
}

char *ZIL_MAP_CHARS::MapText(const ZIL_ICHAR *mapped, char *unMapped, int allocate)
{
	static char ZIL_FARDATA mapTextBuffer[256];
	if (!mapped)
		return(ZIL_NULLP(char));

	char *tmpStr = unMapped;
	int length = sizeof(ZIL_ICHAR)*ZIL_INTERNATIONAL::strlen(mapped)+1;
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
	wcstombs(tmpStr, mapped, length);
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
	mbstowcs(tmpStr, unMapped, length);
	return (tmpStr);
}

int ZIL_MAP_CHARS::mblen(const char *hardware)
{
	if (this == ZIL_NULLP(ZIL_MAP_CHARS))		// PUNT for startup
		return (1);

	if (!toUnicodeDefault)
		return (1);
	ZIL_UINT8 tmp = *hardware;
	for (int i=0; i < leadinSize; i++)
		if (leadins[i].min <= tmp && tmp <= leadins[i].max)
			return (2);
	return (1);
}

// An ANSI wcstombs for Unicode to local platform character mapping.
int ZIL_MAP_CHARS::wcstombs(char *s, const ZIL_ICHAR *pwcs, int n)
{
	if (n < 0)
		n = ZIL_INTERNATIONAL::strlen(pwcs)*sizeof(ZIL_ICHAR)+1;
	int i = 0;
	for (; *pwcs && i < n; pwcs++)
	{
		char *tmp = MapChar(*pwcs);
		if (i + mblen(tmp) >= n)
			break;
		while (*tmp)
			s[i++] = *tmp++;
	}
	if (!*pwcs && i < n)
		s[i] = 0;
	return (i);
}

// An ANSI mbstowcs for local platform character to Unicode mapping.
int ZIL_MAP_CHARS::mbstowcs(ZIL_ICHAR *pwcs, const char *s, int n)
{
	if (n < 0)
		n = ::strlen(s);
	int i = 0;
	for (; *s && i < n; i++)
	{
		pwcs[i] = UnMapChar(s);
		s += mblen(s);
	}
	if (!*s && i < n)
		pwcs[i] = 0;
	return (i);
}
