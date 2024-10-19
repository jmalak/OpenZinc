//	Zinc Interface Library - G_I18N.CPP
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
#	include <stdlib.h>
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
}
#endif
#include "ui_win.hpp"

#define CP_ISO8859_1	1
#define CP_UNICODE	2
#define CP_OTHER	3

//--- Default Locale/Charmap ----------------------------------------

#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))

#include "loc_def.cpp"
#if defined(ZIL_MSDOS) || defined(ZIL_OS2)
#	include "map_ibm.cpp"
#elif defined(ZIL_MACINTOSH)
#	include <OSUtils.h>
#	include "map_mac.cpp"
#elif defined(ZIL_NEXTSTEP)
#	include "map_next.cpp"
#endif

//--- Canonical Locale -------------------------------------------------------

static ZIL_ICHAR ZIL_FARDATA _canonicalIsoLocaleName[] = { 'z', 'a', 'f', 0 };

static ZIL_ICHAR ZIL_FARDATA _canonicalBnumParen[] = { 0 };
static ZIL_ICHAR ZIL_FARDATA _canonicalTimeStringFormat[] = { '%', 'H', ':', '%', 'M', ':', '%', 'S', '.', '%', 'K', 0 };
static ZIL_ICHAR ZIL_FARDATA _canonicalDateStringFormat[] = { '%', 'Y', '-', '%', 'm', '-', '%', 'd', 0 };
static ZIL_ICHAR ZIL_FARDATA _canonicalDateTimeStringFormat[] = { '%', 'Y', '-', '%', 'm', '-', '%', 'd', ' ', '%', 'H', ':', '%', 'M', ':', '%', 'S', '.', '%', 'K', 0 };
static ZIL_ICHAR ZIL_FARDATA _canonicalDefDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static ZIL_LOCALE_ELEMENT ZIL_FARDATA _canonicalLocaleElement =
{
	{ '.', 0 },	// decimalSeparator
	{ '.', 0 },	// monDecimalSeparator
	{ 0 },		// thousandsSeparator
	{ 0 },		// monthousandsSeparator
	{ 0 },		// currencySymbol
	{ 0 },		// grouping
	{ 0 },		// monGrouping
	{ 0 },		// intCurrencySymbol
	1,		// posCurrencyPrecedes
	1,		// negCurrencyPrecedes
	2,		// fractionDigits
	2,		// intFractionDigits
	{ 0 },		// positiveSign
	1,		// posSignPreceides
	0,		// posSpaceSeparation
	{ '-', 0 },	// negativeSign
	1,		// negSignPreceides
	0,		// negSpaceSeparation
	_canonicalBnumParen,
	_canonicalBnumParen,
	_canonicalTimeStringFormat,
	_canonicalDateStringFormat,
	_canonicalDateTimeStringFormat,
	_canonicalTimeStringFormat,
	_canonicalDefDigits,
	_canonicalDefDigits,
	{ ':', 0 },
	{ '-', 0 },
	DTF_ASIAN_FORMAT,
	TMF_TWENTY_FOUR_HOUR,
	2299160L,	// Oct 4, 1582
	11,		// to 2299171L	Oct 15, 1582
	0,
	ZIL_NULLP(ZIL_ERA_TABLE)
};

//----------------------------------------------------------------------
#if defined(ZIL_MSDOS) || defined(ZIL_OS2) || defined(ZIL_MACINTOSH) || defined(ZIL_NEXTSTEP)
ZIL_MAP_CHARS *ZIL_INTERNATIONAL::defaultCharMap =
	new ZIL_MAP_CHARS(_MapName, _ISOToLocal, _LocalToISO, FALSE);
#else
ZIL_MAP_CHARS *ZIL_INTERNATIONAL::defaultCharMap =
	new ZIL_MAP_CHARS(_blankString, ZIL_NULLP(void), ZIL_NULLP(void), FALSE);
#endif

//----------------------------------------------------------------------
const ZIL_LOCALE *ZIL_INTERNATIONAL::canonicalLocale;
const ZIL_LOCALE *ZIL_INTERNATIONAL::defaultLocale;

void ZIL_INTERNATIONAL::DefaultI18nInitialize(void)
{
#if defined(ZIL_MACINTOSH)
	// Fix for time value.
	MachineLocation machLoc;
	ReadLocation(&machLoc);
	long secondsEastGMT = (machLoc.u.gmtDelta << 8) >> 8;
//	long secondsEastGMT = (machLoc.gmtFlags.gmtDelta << 8) >> 8;		// This is for pre-2.0 Universal Headers
	minutesWestGMT = -secondsEastGMT / 60L;
#elif defined(__ZTC__)
	// Fix for time value.
	char *tmp = ::getenv("TZ");
	minutesWestGMT = (tmp ? 60*(tmp[2]-'0') : 0);
#else
	minutesWestGMT = 0;		// Handled by the C library
#endif
	ZIL_LOCALE_MANAGER::SetLocale(_defaultIsoLocaleName, &_defaultLocaleElement);
	defaultLocale = ZIL_LOCALE_MANAGER::UseLocale(_defaultIsoLocaleName);
	ZIL_LOCALE_MANAGER::SetLocale(_canonicalIsoLocaleName, &_canonicalLocaleElement);
	canonicalLocale = ZIL_LOCALE_MANAGER::UseLocale(_canonicalIsoLocaleName);
}
