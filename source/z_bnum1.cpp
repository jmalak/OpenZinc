//	Zinc Interface Library - Z_BNUM1.HPP
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
}
#include "ui_gen.hpp"

static ZIL_ICHAR _bnumfFormat[]	= { '%','l','f', 0 };
static ZIL_ICHAR _bnumnfFormat[]	= { '%','-','.','*','l','f', 0 };

ZIL_BIGNUM::ZIL_BIGNUM(ZIL_RBIGNUM value)
{
	MakeStack();
	myLocale = ZIL_LOCALE_MANAGER::UseLocale();
	Import(value);
}

/*--------------------------------------------------------------------------*/
/* Support for mixed mode (ZIL_BIGNUM op double) operations */
void ZIL_BIGNUM::Export(ZIL_RBIGNUM *value)
{
	ZIL_RBIGNUM r;
	ZIL_ICHAR tmp[ZIL_NUMBER_WHOLE+ZIL_NUMBER_DECIMAL+4];

	const ZIL_LOCALE *save = myLocale;
	myLocale = canonicalLocale;
	Export(tmp, NMF_DECIMAL(ZIL_NUMBER_DECIMAL));
	myLocale = save;
	sscanf(tmp, _bnumfFormat, &r);
	*value = r;
}

void ZIL_BIGNUM::Rtob(ZIL_RBIGNUM value)
{
	ZIL_ICHAR tmp[ZIL_NUMBER_WHOLE+ZIL_NUMBER_DECIMAL+4];

	sprintf(tmp, _bnumnfFormat, ZIL_NUMBER_DECIMAL, value);
	const ZIL_LOCALE *save = myLocale;
	myLocale = canonicalLocale;
	Import(tmp);
	myLocale = save;
}

NMI_RESULT ZIL_BIGNUM::Import(ZIL_RBIGNUM value)
{
	this->Rtob(value);
	return NMI_OK;
}
