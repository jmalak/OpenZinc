//	Zinc Interface Library - Z_LOCALE.CPP
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
#include "ui_gen.hpp"

#define CP_ISO8859_1	1
#define CP_UNICODE	2
#define CP_OTHER	3

static ZIL_ICHAR _pathLocale[] =
{
	'L','O','C','A','L','E',
	0
};
ZIL_LOCALE_MANAGER localeManager;
static ZIL_ICHAR _defaultName[12] = { 0 };
static ZIL_ICHAR _defaultOSName[12] = { 0 };

// ----- ZIL_LOCALE ---------------------------------------------------

ZIL_LOCALE::ZIL_LOCALE(void) : ZIL_I18N()
{
	className = _pathLocale;
	pathName = _pathLocale;
}

ZIL_LOCALE::~ZIL_LOCALE(void)
{
	ZIL_LOCALE::DeleteData();
}

void ZIL_LOCALE::AssignData(const ZIL_I18N *_data)
{
	data = ((ZIL_LOCALE *)_data)->data;
}

void ZIL_LOCALE::DeleteData(void)
{
	if (allocated)
	{
		delete []data.bnumLeftParen;
		delete []data.bnumRightParen;
		delete []data.timeStringFormat;
		delete []data.dateStringFormat;
		delete []data.dateTimeStringFormat;
		delete []data.time12StringFormat;
		delete []data.defDigits;
		delete []data.altDigits;
		delete []data.eraTable;
	}
}

//--- Locale Setup ---------------------------------------------------------

ZIL_LOCALE_MANAGER::ZIL_LOCALE_MANAGER(void)
{
	initialized = TRUE;
	defaultName = _defaultName;
	defaultOSName = _defaultOSName;
}

const ZIL_LOCALE *ZIL_LOCALE_MANAGER::UseLocale(const ZIL_ICHAR *localeName)
{
	if (localeManager.First())
		return (ZIL_LOCALE *)localeManager.UseI18N(((ZIL_LOCALE *)localeManager.First())->pathName, localeName);
	return ZIL_NULLP(ZIL_LOCALE);
}

void ZIL_LOCALE_MANAGER::SetLocale(const ZIL_ICHAR *localeName,
		ZIL_LOCALE_ELEMENT *defaultLocale)
{
	ZIL_LOCALE *def = new ZIL_LOCALE;
	ZIL_INTERNATIONAL::strcpy(def->name, localeName);
	def->data = *defaultLocale;
	def->allocated = FALSE;
	def->defaults = FALSE;
	def->useCount = 0;
	localeManager.Add(def);
	if (_defaultName[0] == 0)
	{
		ZIL_INTERNATIONAL::strcpy(_defaultName, localeName);
		def->defaults = TRUE;
	}
}

//----------------------------------------------------------------------------

#if defined(ZIL_LOAD)
void ZIL_LOCALE::ClassLoadData(ZIL_STORAGE_OBJECT_READ_ONLY *file)
{
	// Load in data from the current directory.
	int i; ZIL_INT16 value;
	file->Load(data.decimalSeparator, 4);
	file->Load(data.monDecimalSeparator, 4);
	file->Load(data.thousandsSeparator, 4);
	file->Load(data.monThousandsSeparator, 4);
	file->Load(data.currencySymbol, 8);
	for (i = 0; i < 10; i++)
	{
		ZIL_INT8 tGrouping;
		file->Load(&tGrouping);
		data.grouping[i] = (char)tGrouping;
	}
	for (i = 0; i < 10; i++)
	{
		ZIL_INT8 tGrouping;
		file->Load(&tGrouping);
		data.monGrouping[i] = (char)tGrouping;
	}
	file->Load(data.intCurrencySymbol, 5);
	file->Load(&value); data.posCurrencyPrecedes = value;
	file->Load(&value); data.negCurrencyPrecedes = value;
	file->Load(&value); data.fractionDigits = value;
	file->Load(&value); data.intFractionDigits = value;
	file->Load(data.positiveSign, 4);
	file->Load(&value); data.posSignPrecedes = value;
	file->Load(&value); data.posSpaceSeparation = value;
	file->Load(data.negativeSign, 4);
	file->Load(&value); data.negSignPrecedes = value;
	file->Load(&value); data.negSpaceSeparation = value;

	file->Load(&data.bnumLeftParen);
	file->Load(&data.bnumRightParen);
	file->Load(&data.timeStringFormat);
	file->Load(&data.dateStringFormat);
	file->Load(&data.dateTimeStringFormat);
	file->Load(&data.time12StringFormat);
	file->Load(&data.defDigits);
	file->Load(&data.altDigits);

	file->Load(data.timeSeparator, 4);
	file->Load(data.dateSeparator, 4);
	file->Load(&value); data.defaultDateFlags = value;
	file->Load(&value); data.defaultTimeFlags = value;
	file->Load(&data.beginGregorian);
	file->Load(&data.skipGregorian);
	file->Load(&data.eraTableLen);
	data.eraTable = ZIL_NULLP(ZIL_ERA_TABLE);
	if (data.eraTableLen)
	{
		data.eraTable = new ZIL_ERA_TABLE[data.eraTableLen];
		for (i=0; i < data.eraTableLen; i++)
		{
			file->Load(&data.eraTable[i].direction);
			file->Load(&data.eraTable[i].offset);
			file->Load(&data.eraTable[i].startDate);
			file->Load(&data.eraTable[i].endDate);
			file->Load(data.eraTable[i].eraName, 6);
			file->Load(data.eraTable[i].eraFormat, 8);
		}
	}
	error = file->objectError;
}
#endif

#if defined(ZIL_STORE)
void ZIL_LOCALE::ClassStoreData(ZIL_STORAGE_OBJECT *file)
{
	// Store locale to the current directory.
	int i; ZIL_INT16 value;
	file->Store(data.decimalSeparator);
	file->Store(data.monDecimalSeparator);
	file->Store(data.thousandsSeparator);
	file->Store(data.monThousandsSeparator);
	file->Store(data.currencySymbol);
	for (i = 0; i < 10; i++)
		file->Store((ZIL_INT8)data.grouping[i]);
	for (i = 0; i < 10; i++)
		file->Store((ZIL_INT8)data.monGrouping[i]);
	file->Store(data.intCurrencySymbol);
	value = data.posCurrencyPrecedes; file->Store(value);
	value = data.negCurrencyPrecedes; file->Store(value);
	value = data.fractionDigits; file->Store(value);
	value = data.intFractionDigits; file->Store(value);
	file->Store(data.positiveSign);
	value = data.posSignPrecedes; file->Store(value);
	value = data.posSpaceSeparation; file->Store(value);
	file->Store(data.negativeSign);
	value = data.negSignPrecedes; file->Store(value);
	value = data.negSpaceSeparation; file->Store(value);

	file->Store(data.bnumLeftParen);
	file->Store(data.bnumRightParen);
	file->Store(data.timeStringFormat);
	file->Store(data.dateStringFormat);
	file->Store(data.dateTimeStringFormat);
	file->Store(data.time12StringFormat);
	file->Store(data.defDigits);
	file->Store(data.altDigits);

	file->Store(data.timeSeparator);
	file->Store(data.dateSeparator);
	value = data.defaultDateFlags; file->Store(value);
	value = data.defaultTimeFlags; file->Store(value);
	file->Store(data.beginGregorian);
	file->Store(data.skipGregorian);
	file->Store(data.eraTableLen);
	for (i=0; i < data.eraTableLen; i++)
	{
		file->Store(data.eraTable[i].direction);
		file->Store(data.eraTable[i].offset);
		file->Store(data.eraTable[i].startDate);
		file->Store(data.eraTable[i].endDate);
		file->Store(data.eraTable[i].eraName);
		file->Store(data.eraTable[i].eraFormat);
	}
	error = file->objectError;
}
#endif
