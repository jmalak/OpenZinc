//	Zinc Interface Library - Z_DATE.CPP
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
#if defined(ZIL_NEXTSTEP)
extern "C"
{
#include <stdlib.h>
}
#endif

// ----- ZIL_DATE ------------------------------------------------------------

ZIL_DATE::ZIL_DATE(void) :
	ZIL_UTIME()
{
	usec = 0;
}

ZIL_DATE::ZIL_DATE(const ZIL_DATE &date) :
	ZIL_UTIME(date)
{
}

ZIL_DATE::ZIL_DATE(int year, int month, int day) :
	ZIL_UTIME(year, month, day, 0, 0, 0, 0)
{
}

ZIL_DATE::ZIL_DATE(const ZIL_ICHAR *string, DTF_FLAGS dtFlags) :
	ZIL_UTIME()
{
	Import(string, dtFlags);
}

ZIL_DATE::ZIL_DATE(int packedDate) :
	ZIL_UTIME()
{
	Import(packedDate);
}

void ZIL_DATE::Export(ZIL_ICHAR *string, DTF_FLAGS dtFlags)
{
	static ZIL_ICHAR _yStr[] = { '%', 'y', 0 };
	static ZIL_ICHAR YStr[] = { '%', 'Y', 0 };
	static ZIL_ICHAR _bStr[] = { '%', 'b', 0 };
	static ZIL_ICHAR BStr[] = { '%', 'B', 0 };
	static ZIL_ICHAR _gStr[] = { '%', 'g', 0 };
	static ZIL_ICHAR GStr[] = { '%', 'G', 0 };
	static ZIL_ICHAR mStr[] = { '%', 'v', 0 };
	static ZIL_ICHAR mZStr[] = { '%', 'm', 0 };
	static ZIL_ICHAR _aStr[] = { '%', 'a', ' ', 0 };
	static ZIL_ICHAR AStr[] = { '%', 'A', ' ', 0 };
	static ZIL_ICHAR dStr[] = { '%', 'e', 0 };
	static ZIL_ICHAR dZStr[] = { '%', 'd', 0 };
	static ZIL_ICHAR _dateBlankSeparator[] 	= { ' ', 0 };
	static ZIL_ICHAR _dateDashSeparator[] 	= { '-', 0 };
	static ZIL_ICHAR _dateSlashSeparator[] 	= { '/', 0 };
	static ZIL_ICHAR _dateCommaSeparator[] 	= { ',',' ', 0 };

	if (jday == 0 && usec < 0)
	{
		// Unanswered value
		*string = 0;
		return;
	}
	if (!myLocale)
	{
		myLocale = ZIL_LOCALE_MANAGER::UseLocale();
		SetLanguage();
		if (!myLocale)
			return;
	}
	if (!FlagSet(dtFlags, DTF_US_FORMAT|DTF_EUROPEAN_FORMAT|DTF_ASIAN_FORMAT))
	{
		if (myLocale->data.defaultDateFlags == -1)
		{
			ZIL_UTIME::Export(string, 128, myLocale->data.dateStringFormat);
			return;
		}
		dtFlags |= myLocale->data.defaultDateFlags;
	}

	// Determine the separator character.
	const ZIL_ICHAR *yearSeparator, *monthSeparator, *daySeparator;
	if (FlagSet(dtFlags, DTF_MILITARY_FORMAT))
	{
		dtFlags &= (DTF_MILITARY_FORMAT | DTF_SHORT_MONTH | DTF_SHORT_YEAR |
			DTF_SYSTEM | DTF_UPPER_CASE | DTF_ZERO_FILL);
		dtFlags |= DTF_ALPHA_MONTH;
		if (FlagSet(dtFlags, DTF_SHORT_MONTH | DTF_SHORT_YEAR))
			dtFlags |= DTF_SHORT_MONTH | DTF_SHORT_YEAR;
		yearSeparator=monthSeparator=daySeparator= _dateBlankSeparator;
	}
	else if (FlagSet(dtFlags, DTF_DASH))
		yearSeparator=monthSeparator=daySeparator= _dateDashSeparator;
	else if (FlagSet(dtFlags, DTF_SLASH))
		yearSeparator=monthSeparator=daySeparator= _dateSlashSeparator;
	else if (FlagSet(dtFlags, DTF_ALPHA_MONTH | DTF_SHORT_MONTH) &&
		 (!FlagSet(dtFlags, DTF_ASIAN_FORMAT) &&
		  FlagSet(dtFlags, DTF_EUROPEAN_FORMAT|DTF_US_FORMAT)))
	{
		yearSeparator = myLocale->data.dateSeparator;
		daySeparator = _dateCommaSeparator;
		monthSeparator = _dateBlankSeparator;
	}
	else
		yearSeparator=monthSeparator=daySeparator= myLocale->data.dateSeparator;

	int doZeroFill = FlagSet(dtFlags, DTF_ZERO_FILL);

	// Determine the year value.
	ZIL_ICHAR *yearStr = (FlagSet(dtFlags, DTF_SHORT_YEAR) ? _yStr : YStr);

	// Determine the month value.
	ZIL_ICHAR *monthStr = (FlagSet(dtFlags, DTF_SHORT_MONTH) ?
			     (FlagSet(dtFlags, DTF_MILITARY_FORMAT)?_gStr:_bStr):
			     (FlagSet(dtFlags, DTF_ALPHA_MONTH) ? BStr :
			      (doZeroFill ? mZStr : mStr)));

	// Add the day of week value to the date.
	ZIL_ICHAR *dayStr = (FlagSet(dtFlags, DTF_SHORT_DAY) ?
			   (FlagSet(dtFlags, DTF_MILITARY_FORMAT) ?GStr:_aStr) :
			   (FlagSet(dtFlags, DTF_DAY_OF_WEEK) ? AStr : _blankString));

	ZIL_ICHAR tmpFormat[64];
	strcpy(tmpFormat, dayStr);
	// Format the date according to the country information.
	if (FlagSet(dtFlags, DTF_ASIAN_FORMAT))
	{
		strcat(tmpFormat, yearStr);
		strcat(tmpFormat, yearSeparator);
		strcat(tmpFormat, monthStr);
		strcat(tmpFormat, monthSeparator);
		strcat(tmpFormat, (doZeroFill ? dZStr : dStr));
	}
	else if (FlagSet(dtFlags, DTF_EUROPEAN_FORMAT | DTF_MILITARY_FORMAT))
	{
		strcat(tmpFormat, (doZeroFill ? dZStr : dStr));
		strcat(tmpFormat, monthSeparator);
		strcat(tmpFormat, monthStr);
		strcat(tmpFormat, daySeparator);
		strcat(tmpFormat, yearStr);
	}
	else if (FlagSet(dtFlags, DTF_US_FORMAT))
	{
		strcat(tmpFormat, monthStr);
		strcat(tmpFormat, monthSeparator);
		strcat(tmpFormat, (doZeroFill ? dZStr : dStr));
		strcat(tmpFormat, daySeparator);
		strcat(tmpFormat, yearStr);
	}
	else
		strcat(tmpFormat, myLocale->data.dateStringFormat);

	ZIL_UTIME::Export(string, 128, tmpFormat);
	if (FlagSet(dtFlags, DTF_UPPER_CASE))
		strupr(string);
}

void ZIL_DATE::Export(int *packedDate)
{
	int year, month, day;

	if (jday == 0 && usec < 0)
		*packedDate = -1;
	else
	{
		int oldBasis = basisYear;
		basisYear = 0;
		ConvertJday(&year, &month, &day, ZIL_NULLP(int));
		basisYear = oldBasis;
		*packedDate = ((year - 1980) << 9) | (month << 5) | day;
	}
}

DTI_RESULT ZIL_DATE::Import(const ZIL_ICHAR *string, DTF_FLAGS dtFlags)
{
	// Check for a system date.
	if (!string || !string[0])
	{
		if (FlagSet(dtFlags, DTF_SYSTEM))
			return (ZIL_DATE::Import());
		jday = 0;
		usec = -1;
		return (DTI_OK);
	}
	if (!myLocale)
	{
		myLocale = ZIL_LOCALE_MANAGER::UseLocale();
		SetLanguage();
		if (!myLocale)
			return (DTI_INVALID);
	}
	if (!FlagSet(dtFlags, DTF_US_FORMAT|DTF_EUROPEAN_FORMAT|DTF_ASIAN_FORMAT))
	{
		if (myLocale->data.defaultDateFlags == -1)
		{
			if (ZIL_UTIME::Import(string, myLocale->data.dateStringFormat) == ZIL_NULLP(ZIL_ICHAR))
				return (DTI_INVALID);
			usec = 0;
			return (DTI_OK);
		}
		dtFlags |= myLocale->data.defaultDateFlags;
	}

	if (FlagSet(dtFlags, DTF_MILITARY_FORMAT))
	{
		dtFlags &= (DTF_MILITARY_FORMAT | DTF_SHORT_MONTH | DTF_SHORT_YEAR |
			DTF_SYSTEM | DTF_UPPER_CASE | DTF_ZERO_FILL);
		dtFlags |= DTF_ALPHA_MONTH;
		if (FlagSet(dtFlags, DTF_SHORT_MONTH | DTF_SHORT_YEAR))
			dtFlags |= DTF_SHORT_MONTH | DTF_SHORT_YEAR;
	}
//	if (FlagSet(dtFlags, DTF_SHORT_YEAR))
	{
		ZIL_DATE today;
		int y1;
		today.Export(&y1, ZIL_NULLP(int), ZIL_NULLP(int));
		basisYear = y1 - (y1 % 100);
	}

	// Set up the date variables and determine their proper order.
	int year = 2, month = 1, day = 0;
	if (FlagSet(dtFlags, DTF_ASIAN_FORMAT))
		day = 2, year = 0;
	else if (FlagSet(dtFlags, DTF_EUROPEAN_FORMAT | DTF_MILITARY_FORMAT))
		;// already set
	else if (FlagSet(dtFlags, DTF_US_FORMAT))
		day = 1, month = 0;
	else
	{
		if (ZIL_UTIME::Import(string, myLocale->data.dateStringFormat) == ZIL_NULLP(ZIL_ICHAR))
			return (DTI_INVALID);
		return (DTI_OK); // ????
	}

	// Parse out the string date.
	static unsigned date[3] = { 0, 0, 0 };
	int alphaCount = 0, i = 0;
	for (; i < 3 && *string; i++)
	{
		while (*string && !IsDigit(*string) && !IsAlpha(*string))
			string++;
		if (IsDigit(*string))
		{
			ZIL_ICHAR *tmp;
			ZIL_UINT32 val = strtoul(string, &tmp, 10);
			if (val > 32767L)
				return (DTI_INVALID);
			date[i] = (unsigned)val;
			string = (const ZIL_ICHAR *)tmp;
		}
		else if (IsAlpha(*string))
		{
			int found = FALSE;
			const ZIL_ICHAR *tString = string;
			while (*string && IsAlpha(*string))
				string++;
			for (int j = 1; j <= 12; j++)
				if (strnicmp(tString, myMonthStrings->data[j].text, (int)(string - tString)) == 0 ||
				    stricmp(tString, myMonthStrings->data[j+12].text) == 0 ||
				    stricmp(tString, myMonthStrings->data[j+24].text) == 0)
				{
					if (found)
					{
						jday = 0;
						usec = (ZIL_INT32)-1;
						return (DTI_AMBIGUOUS);
					}
					date[i] = j;
					if (year == i)
						year = month;
					else if (day == i)
						day = month;
					month = i;
					found = TRUE;
					++alphaCount;
				}
			if (!found)
				i--;
		}
	}
	if (i != 3 || alphaCount > 1)	// We didn't parse out a complete date...
		return (DTI_INVALID);

	// Set the date according to the parsed values.
if (date[year] < 100)
//if (FlagSet(dtFlags, DTF_SHORT_YEAR) && date[year] < 100)
//	if (FlagSet(dtFlags, DTF_SHORT_YEAR))
		date[year] += basisYear;
	if (!(1 <= date[month] && date[month] <= 12))
		return (DTI_INVALID);
	// Set up for the DaysInMonth() call for leap years.
	ZIL_UTIME::Import(date[year], 1, 1, 0, 0, 0, 0);
	if (!(1 <= date[day] && date[day] <= DaysInMonth(date[month])))
		return (DTI_INVALID);
	ZIL_UTIME::Import(date[year], date[month], date[day], 0, 0, 0, 0);
	return (DTI_OK);
}

DTI_RESULT ZIL_DATE::Import(int packedDate)
{
	if (packedDate == -1)
	{
		jday = 0;
		usec = -1;		// This can work only because -1 isn't
		return (DTI_OK);	// a valid packed date.
	}
	ZIL_UTIME::Import(1980 + (((int)(packedDate & 0xFE00)) >> 9),
			 ((int)(packedDate & 0x01E0)) >> 5,
			 packedDate & 0x001F,
			 0, 0, 0, 0);
	return (DTI_OK);
}
