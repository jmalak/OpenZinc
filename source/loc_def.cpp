//	Zinc Interface Library - LOC_US.CPP
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


#if defined(ZIL_UNICODE)
#	define locCodeset	CP_UNICODE
#else
#	define locCodeset	CP_ISO8859_1
#endif

// Locale definitions for the USA (US).
static ZIL_ICHAR ZIL_FARDATA _defaultIsoLocaleName[] = { 'U', 'S', 0 };

static ZIL_ICHAR ZIL_FARDATA _defaultBnumLeftParen[] = { '(', 0 };
static ZIL_ICHAR ZIL_FARDATA _defaultBnumRightParen[] = { ')', 0 };
static ZIL_ICHAR ZIL_FARDATA _defaultTimeStringFormat[] = { '%', 'I', ':', '%', 'M', ':', '%', 'S', ' ', '%', 'p', 0 };
static ZIL_ICHAR ZIL_FARDATA _defaultDateStringFormat[] = { '%', 'm', '/', '%', 'd', '/', '%', 'y', 0 };
static ZIL_ICHAR ZIL_FARDATA _defaultDateTimeStringFormat[] = { '%', 'm', '/', '%', 'd', '/', '%', 'y', ' ', '%', 'I', ':', '%', 'M', ':', '%', 'S', ' ', '%', 'p', 0 };
static ZIL_ICHAR ZIL_FARDATA _defaultDefDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static ZIL_ERA_TABLE ZIL_FARDATA eraTable[] =
{
	{ 1, 1, 1721424L, 0xffffffffL, { 'A','D', 0 }, { '%','E','C',' ','%','E','y', 0 } },
	{ 0, 4713, 0L, 1721423L, { 'B','C', 0 }, { '%','E','y',' ','%','E','C', 0 } }
};
static ZIL_LOCALE_ELEMENT ZIL_FARDATA _defaultLocaleElement =
{
	{ '.', 0 },	// decimalSeparator
	{ '.', 0 },	// monDecimalSeparator
	{ ',', 0 },	// thousandsSeparator
	{ ',', 0 },	// monthousandsSeparator
	{ '$', 0 },	// currencySymbol
	{ 3, 0 },	// grouping
	{ 3, 0 },	// monGrouping
	{ 'U','S','D',' ',0 },	// intCurrencySymbol
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
	_defaultBnumLeftParen,
	_defaultBnumRightParen,
	_defaultTimeStringFormat,
	_defaultDateStringFormat,
	_defaultDateTimeStringFormat,
	_defaultTimeStringFormat,
	_defaultDefDigits,
	_defaultDefDigits,
	{ ':', 0 },
	{ '-', 0 },
	DTF_US_FORMAT,
	TMF_TWELVE_HOUR,
	2299160L,	// Oct 4, 1582
	11,		// to 2299171L	Oct 15, 1582
	LENGTHOF(eraTable),
	eraTable
};

#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_CURSES)
int ZIL_LOCALE::oemCountryCode = 1;
#elif defined(ZIL_MACINTOSH)
int ZIL_LOCALE::oemCountryCode = 0;
#endif
