//	Zinc Interface Library - I_STR1.CPP
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


#include <stdlib.h>
#include "ui_gen.hpp"

#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))

ZIL_ICHAR *ZIL_INTERNATIONAL::strncpy(ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int n)
{
	int i = 0;
	for (; n && s2[i]; i++, n--)
		s1[i] = s2[i];
	for (; n; i++, n--)
		s1[i] = 0;
	return (s1);
}

ZIL_ICHAR *ZIL_INTERNATIONAL::strchr(const ZIL_ICHAR *s, int c)
{
	int i = 0;
	for (; s[i] && s[i] != (ZIL_ICHAR)c; i++)
		;
	if (s[i] == (ZIL_ICHAR)c)
		return ((ZIL_ICHAR *)(s + i));
	return (ZIL_NULLP(ZIL_ICHAR));
}

ZIL_ICHAR *ZIL_INTERNATIONAL::strcpy(ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
{
	int i = 0;
	for (; s2[i]; i++)
		s1[i] = s2[i];
	s1[i] = 0;
	return (s1);
}

ZIL_ICHAR *ZIL_INTERNATIONAL::strcat(ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
{
	int i = 0;
	for (; s1[i]; i++)
		;
	for (int j=0; s2[j]; i++, j++)
		s1[i] = s2[j];
	s1[i] = 0;
	return (s1);
}

ZIL_ICHAR *ZIL_INTERNATIONAL::strrchr(const ZIL_ICHAR *s, int c)
{
	int i = 0;
	for (; s[i]; i++)
		;
	for (; i && s[i] != (ZIL_ICHAR)c; i--)
		;
	if (s[i] == (ZIL_ICHAR)c)
		return ((ZIL_ICHAR *)(s + i));
	return (ZIL_NULLP(ZIL_ICHAR));
}

int ZIL_INTERNATIONAL::streq(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
{
	int i = 0;
	for (; s1[i] && s2[i] && s1[i] == s2[i]; i++)
		;
	return ((ZIL_INT16)s1[i] - (ZIL_INT16)s2[i]);
}

int ZIL_INTERNATIONAL::strneq(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int n)
{
	int i = 0;
	for (; n && s1[i] && s2[i] && s1[i] == s2[i]; i++, n--)
		;
	return (n ? (signed)s1[i] - (signed)s2[i] : 0);
}

ZIL_ICHAR *ZIL_INTERNATIONAL::ISOtoUNICODE(const char *isoString, ZIL_ICHAR *retValue)
{
	if (!isoString)
		return ZIL_NULLP(ZIL_ICHAR);
	int i, len = ::strlen(isoString);
	if (!retValue)
		retValue = new ZIL_ICHAR[len+1];
	for (i=0; i <= len; i++)
		retValue[i] = (ZIL_UINT8)isoString[i];
	return (retValue);
}
