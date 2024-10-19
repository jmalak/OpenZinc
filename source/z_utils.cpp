//	Zinc Interface Library - Z_UTILS.CPP
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


extern "C"
{
#include <stdio.h>
#include <stdarg.h>
}
#if !(defined(__GNUC__) && defined(__DVX__))
extern "C"
{
#include <locale.h>
}
#endif
#include "ui_gen.hpp"
#if defined(_SUNOS4)
#	include <malloc.h>
#endif

#if defined(__GNUC__) && defined(__DVX__)
static struct lconv _lconv = {
	".", ",", "\3", "USD", "$", ".", ",", "\3",
	"", "-", 2, 2, 1, 0, 1, 0, 0, 0
};

struct lconv *localeconv( void )
{
	return &_lconv;
}

ZIL_ICHAR *setlocale( int category, const ZIL_ICHAR *locale )
{
}

#endif

ZIL_ICHAR *ZIL_INTERNATIONAL::strdup(const ZIL_ICHAR *srcString)
{
	// strdup using new instead of malloc.
	if (!srcString)
		return (ZIL_NULLP(ZIL_ICHAR));
	ZIL_ICHAR *destString = new ZIL_ICHAR[strlen(srcString) + 1];
	strcpy(destString, srcString);
	return (destString);
}

int ZIL_INTERNATIONAL::strlen(const ZIL_ICHAR *string)
{
	// Return the length of a string.
	if (!string)
		return (0);

#if defined(ZIL_UNICODE)
	int i = 0;
	for (; string[i]; i++)
		;
	return (i);
#else
	return (::strlen(string));
#endif
}

void ZIL_INTERNATIONAL::strrepc(ZIL_ICHAR *string, int c, int repc)
{
	if (!string)
		return;

	// Replace a character (c) in a string to a new character (repc).
	while (*string)
	{
		if (*string == c)
			*string = repc;
		string++;
	}
}

void ZIL_INTERNATIONAL::strstrip(ZIL_ICHAR *string, ZIL_ICHAR c)
{
	if (!string)
		return;

	// strip all occurances of c out of string
	ZIL_ICHAR *src = string;
	ZIL_ICHAR *dest = string;
	do {
		while (*src == c)
			src++;
	} while ((*dest++ = *src++) != 0);
}

//	ZIL_INTERNATIONAL::WildStrcmp() is a strcmp() that handles
//	the * and ? wildcards in pattern.
int ZIL_INTERNATIONAL::WildStrcmp(const ZIL_ICHAR *str, const ZIL_ICHAR *pattern)
{
	for ( ;  *str && (*str == *pattern || *pattern == '?' || *pattern == '*');
		str++, pattern++)
		if (*pattern == '*') {
			// Compress multiple astrisks, eg: "**".
			while (*++pattern == '*')
				;	// Update pattern count.
			if (!*pattern)
				return(0);		// pattern ended in *.
			const ZIL_ICHAR *nextTest = str;
			do {
				nextTest = strchr(nextTest, *pattern);
				if (!nextTest)
				{
					pattern--;
					break;
				}
				if (!WildStrcmp(nextTest, pattern)) {
					str = nextTest;
					break;
				}
				nextTest++;
			} while (nextTest);
		}
	while (*pattern == '*')
		pattern++;
	return *str - *pattern;
}

int ZIL_INTERNATIONAL::stricmp(const ZIL_ICHAR *a, const ZIL_ICHAR *b)
{
	ZIL_ICHAR la, lb;

	for (;;)
	{
		la = *a; lb = *b;
		if (IsUpper(la))
			la = ToLower(la);
		if (IsUpper(lb))
			lb = ToLower(lb);
	if (la != lb || !la)
		break;
		a++; b++;
	}
	return la - lb;
}

ZIL_ICHAR *ZIL_INTERNATIONAL::strupr(ZIL_ICHAR *a)
{
	ZIL_ICHAR *b = a;
	while (*a)
	{
		if (IsLower(*a))
			*a = ToUpper(*a);
		a++;
	}
	return (b);
}

ZIL_ICHAR *ZIL_INTERNATIONAL::strlwr(ZIL_ICHAR *a)
{
	ZIL_ICHAR *b = a;
	while (*a)
	{
		if (IsUpper(*a))
			*a = ToLower(*a);
		a++;
	}
	return (b);
}

int ZIL_INTERNATIONAL::strnicmp(const ZIL_ICHAR *a, const ZIL_ICHAR *b, int n)
{
	ZIL_ICHAR la, lb;

	for (;;)
	{
		la = *a; lb = *b;
		if (IsUpper(la))
			la = ToLower(la);
		if (IsUpper(lb))
			lb = ToLower(lb);
	if (la != lb || !la || !n)
		break;
		a++; b++; n--;
	}
	if (n)
		return (la - lb);
	return (0);
}

#if defined(_SUNOS4)
void *memmove(void *s1, const void *s2, size_t n)
{
	char *tmp = malloc(n);
	memcpy(tmp, s2, n);
	memcpy(s1, tmp, n);
	free (tmp);
	return(s1);
}

#elif defined(ZIL_MACINTOSH)
void ZIL_INTERNATIONAL::pstrcat(StringPtr dst, StringPtr src)
{
	BlockMove(src + 1, dst + *dst + 1, *src);
	*dst += *src;
}

void ZIL_INTERNATIONAL::pstrinsert(StringPtr dst, StringPtr src)
{
	BlockMove(dst + 1, dst + *src + 1, *dst);
	BlockMove(src + 1, dst + 1, *src);
	*dst += *src;
}

void ZIL_INTERNATIONAL::GetPathNameFromDirID(StringPtr fullPathName, long dirID, short vRefNum)
{
	DirInfo block;
	Str255 directoryName;

	fullPathName[0] = '\0';
	block.ioDrParID = dirID;
	block.ioNamePtr = directoryName;
	do
	{
		block.ioVRefNum = vRefNum;
		block.ioFDirIndex = -1;
		block.ioDrDirID = block.ioDrParID;
		PBGetCatInfo((CInfoPBRec *)&block, FALSE);
		pstrcat(directoryName, (StringPtr)"\p:");
		pstrinsert(fullPathName, directoryName);
	} while (block.ioDrDirID != 2);
}
#endif

void ZIL_INTERNATIONAL::StripHotMark(ZIL_ICHAR *fillLine)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	int length = strlen(fillLine);
	ZIL_ICHAR *hotChar = fillLine;
	for (int i=0; i < length; i++, hotChar++)
 	{
		if ((fillLine[i] == ZIL_HOTMARK && fillLine[i+1] == ZIL_HOTMARK) ||
			(fillLine[i] == '~' && fillLine[i+1] == '~'))
		{
 			memmove(hotChar, hotChar + 1, strlen(hotChar) * sizeof(ZIL_ICHAR));
			i++, hotChar++;
		}
 		else if (fillLine[i] == ZIL_HOTMARK || fillLine[i] == '~')
 			memmove(hotChar, hotChar + 1, strlen(hotChar) * sizeof(ZIL_ICHAR));
	}
#else
	if (!fillLine)
		return;

	for (ZIL_ICHAR *hotChar = strchr(fillLine, ZIL_HOTMARK); hotChar;
		hotChar = strchr(++hotChar, ZIL_HOTMARK))
	{
		memmove(hotChar, hotChar + 1, strlen(hotChar) * sizeof(ZIL_ICHAR));
		if (*hotChar == ZIL_HOTMARK)
			++hotChar;
	}
#endif
}

void ZIL_INTERNATIONAL::itoa(ZIL_INT32 value, ZIL_ICHAR *string, int radix, int pad)
{
	static ZIL_ICHAR _digits[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'a', 'b', 'c', 'd', 'e', 'f'
	};

	ZIL_UINT32 tmpValue;
	if (value < 0 && radix == 10)
	{
		*string++ = '-';
		tmpValue = (ZIL_UINT32)-value;
	}
	else
		tmpValue = value;
	ZIL_ICHAR tmpBuf[64];
	int i = 0;
	while (tmpValue || i == 0)
	{
		ZIL_UINT16 digit = (ZIL_UINT16)(tmpValue % radix);
		tmpBuf[i] = _digits[digit];
		i++;
		tmpValue /= radix;
		pad--;
	}
	while (pad > 0)
	{
		tmpBuf[i] = (value < 0 ? _digits[radix] : _digits[0]);
		i++;
		pad--;
	}
	do {
		i--;
		*string++ = tmpBuf[i];
	} while (i > 0);
	*string = 0;
}
