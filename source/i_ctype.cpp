//	Zinc Interface Library - I_CTYPE.CPP
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


#include <stdio.h>
#include <stdarg.h>
#if !(defined(__GNUC__) && defined(__DVX__))
#include <locale.h>
#endif
#include "ui_gen.hpp"
#if defined(_SUNOS4)
#	include <malloc.h>
#endif

// These are the ANSI C ctype.h equivalents for ISO 8859-1.

int ZIL_INTERNATIONAL::IsAscii(ZIL_ICHAR c)
{
	return ((unsigned)c <= (unsigned)0x7f);
}

int ZIL_INTERNATIONAL::IsCntrl(ZIL_ICHAR c)
{
	// This form stops the warnings when compiling with ZIL_ICHAR
	// as an unsigned short.
	return ((c >= (ZIL_ICHAR)0x01 && c <= (ZIL_ICHAR)0x1f) ||
		(c >= (ZIL_ICHAR)0x7f && c <= (ZIL_ICHAR)0x9f) ||
		(c == (ZIL_ICHAR)0x00));
}

int ZIL_INTERNATIONAL::IsDigit(ZIL_ICHAR c)
{
	return (c >= '0' && c <= '9');
}

int ZIL_INTERNATIONAL::IsGraph(ZIL_ICHAR c)
{
	return ((c >= '!' && c <= '~') ||
		(c >= (ZIL_ICHAR)0xa0 && c <= (ZIL_ICHAR)0xff));
}

int ZIL_INTERNATIONAL::IsPrint(ZIL_ICHAR c)
{
	return ((c >= ' ' && c <= '~') ||
		(c >= (ZIL_ICHAR)0xa0) && c <= (ZIL_ICHAR)0xff);
}

int ZIL_INTERNATIONAL::IsPrint(ZIL_RAW_CODE c)
{
	if( c > 0xff )
		return 0;
	return IsPrint((ZIL_ICHAR)(unsigned char)c);
}

int ZIL_INTERNATIONAL::IsSpace(ZIL_ICHAR c)
{
	return (c == ' ' || c == '\f' || c == '\n' || c == '\r' ||
		c == '\t' || c == '\v' || c == (ZIL_ICHAR)0xa0);
}

int ZIL_INTERNATIONAL::IsXDigit(ZIL_ICHAR c)
{
	return ((c >= '0' && c <= '9') ||
		(c >= 'a' && c <= 'f') ||
		(c >= 'A' && c <= 'F'));
}

int ZIL_INTERNATIONAL::IsLower(ZIL_ICHAR c)
{
	return ((c >= 'a' && c <= 'z') ||
		(c >= (ZIL_ICHAR)0xdf && c <= (ZIL_ICHAR)0xf6) ||
		(c >= (ZIL_ICHAR)0xf8 && c <= (ZIL_ICHAR)0xff));
}

int ZIL_INTERNATIONAL::IsUpper(ZIL_ICHAR c)
{
	return ((c >= 'A' && c <= 'Z') ||
		(c >= (ZIL_ICHAR)0xc0 && c <= (ZIL_ICHAR)0xd6) ||
		(c >= (ZIL_ICHAR)0xd8 && c <= (ZIL_ICHAR)0xde));
}

ZIL_ICHAR ZIL_INTERNATIONAL::ToLower(ZIL_ICHAR c)
{
	if (c >= 'A' && c <= 'Z')
		return (c + 0x20);
	if (c >= (ZIL_ICHAR)0xc0 && c <= (ZIL_ICHAR)0xd6)
		return (c + 0x20);
	if (c >= (ZIL_ICHAR)0xd8 && c <= (ZIL_ICHAR)0xde)
		return (c + 0x20);
	return (c);
}

ZIL_ICHAR ZIL_INTERNATIONAL::ToLower(ZIL_RAW_CODE c)
{
	return ToLower((ZIL_ICHAR)(unsigned char)c);
}

ZIL_ICHAR ZIL_INTERNATIONAL::ToUpper(ZIL_ICHAR c)
{
	if (c >= 'a' && c <= 'z')
		return (c - 0x20);
	if (c >= (ZIL_ICHAR)0xe0 && c <= (ZIL_ICHAR)0xf6)
		return (c - 0x20);
	if (c >= (ZIL_ICHAR)0xf8 && c <= (ZIL_ICHAR)0xfe)
		return (c - 0x20);
	return (c);
}

int ZIL_INTERNATIONAL::IsAlnum(ZIL_ICHAR c)
{
	return ((c >= '0' && c <= '9') ||
		(c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= (ZIL_ICHAR)0xc0 && c <= (ZIL_ICHAR)0xd6) ||
		(c >= (ZIL_ICHAR)0xd8 && c <= (ZIL_ICHAR)0xf6) ||
		(c >= (ZIL_ICHAR)0xf8 && c <= (ZIL_ICHAR)0xff));
}

int ZIL_INTERNATIONAL::IsAlpha(ZIL_ICHAR c)
{
	return ((c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= (ZIL_ICHAR)0xc0 && c <= (ZIL_ICHAR)0xd6) ||
		(c >= (ZIL_ICHAR)0xd8 && c <= (ZIL_ICHAR)0xf6) ||
		(c >= (ZIL_ICHAR)0xf8 && c <= (ZIL_ICHAR)0xff));
}

int ZIL_INTERNATIONAL::IsPunct(ZIL_ICHAR c)
{
	return ((c >= '!' && c <= '/') ||
		(c >= ':' && c <= '?') ||
		(c >= '[' && c <= '`') ||
		(c >= '{' && c <= '~') ||
		(c >= (ZIL_ICHAR)0xa1 && c <= (ZIL_ICHAR)0xbf) ||
		(c == (ZIL_ICHAR)0xd7 || c == (ZIL_ICHAR)0xf7));
}

int ZIL_INTERNATIONAL::IsNonspacing(ZIL_ICHAR)
{
	return FALSE;
}

