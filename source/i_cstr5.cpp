//	Zinc Interface Library - I_STR5.CPP
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

int ZIL_INTERNATIONAL::strcmp(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
	{ return (::strcmp(s1, s2)); }

int ZIL_INTERNATIONAL::strncmp(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int n)
	{ return (::strncmp(s1, s2, n)); }

ZIL_ICHAR *ZIL_INTERNATIONAL::strstr(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
	{ return (::strstr((ZIL_ICHAR *)s1, s2)); }
