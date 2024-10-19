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

#include "ui_gen.hpp"

ZIL_ICHAR *ZIL_INTERNATIONAL::strncpy(ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int count)
	{ return (::strncpy(s1, s2, count)); }

ZIL_ICHAR *ZIL_INTERNATIONAL::strcpy(ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
	{ return (::strcpy(s1, s2)); }

ZIL_ICHAR *ZIL_INTERNATIONAL::strcat(ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
	{ return (::strcat(s1, s2)); }

ZIL_ICHAR *ZIL_INTERNATIONAL::strchr(const ZIL_ICHAR *s, int count)
	{ return (::strchr((ZIL_ICHAR *)s, count)); }

ZIL_ICHAR *ZIL_INTERNATIONAL::strrchr(const ZIL_ICHAR *s, int c)
	{ return (::strrchr((ZIL_ICHAR *)s, c)); }

int ZIL_INTERNATIONAL::streq(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
	{ return (::strcmp(s1, s2)); }

int ZIL_INTERNATIONAL::strneq(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int n)
	{ return (::strncmp(s1, s2, n)); }
