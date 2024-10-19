//	Zinc Interface Library - I_CSTR3.CPP
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
#include <stdlib.h>
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
}
#endif
#include "ui_gen.hpp"

long ZIL_INTERNATIONAL::strtol(const ZIL_ICHAR *nptr, ZIL_ICHAR **endptr, int base)
	{ return (::strtol(nptr, endptr, base)); }

unsigned long ZIL_INTERNATIONAL::strtoul(const ZIL_ICHAR *nptr, ZIL_ICHAR **endptr, int base)
#if defined(_SUNOS4)
	{ return ((unsigned long)::strtol(nptr, endptr, base)); }
#else
	{ return (::strtoul(nptr, endptr, base)); }
#endif

