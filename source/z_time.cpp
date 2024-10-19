//	Zinc Interface Library - Z_TIME.CPP
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

// ----- ZIL_TIME ------------------------------------------------------------

static ZIL_ICHAR _timeColonSeparator[]	= { ':', 0 };
static ZIL_ICHAR _timeSpaceString[]	= { ' ', 0 };

ZIL_TIME::ZIL_TIME(void) :
	ZIL_UTIME()
{
	jday = 1;
}

ZIL_TIME::ZIL_TIME(const ZIL_TIME &time) :
	ZIL_UTIME(time)
{
}

ZIL_TIME::ZIL_TIME(int hour, int minute, int second, int hundredth) :
	ZIL_UTIME(0, 0, 1, hour, minute, second, hundredth * 10)
     //??? Changed third parameter to base class from 0 to 1 so that 
     //??? comparisons will work
{
}

ZIL_TIME::ZIL_TIME(const ZIL_ICHAR *string, TMF_FLAGS tmFlags) :
	ZIL_UTIME()
{
	Import(string, tmFlags);
}

ZIL_TIME::ZIL_TIME(int packedTime) :
	ZIL_UTIME()
{
	Import(packedTime);
}

void ZIL_TIME::Export(ZIL_ICHAR *string, TMF_FLAGS tmFlags)
{
	static ZIL_ICHAR hStr[] = { '%', 'h', 0 };
	static ZIL_ICHAR hZStr[] = { '%', 'H', 0 };
	static ZIL_ICHAR iStr[] = { '%', 'i', 0 };
	static ZIL_ICHAR iZStr[] = { '%', 'I', 0 };
	static ZIL_ICHAR mStr[] = { '%', 'M', 0 };
	static ZIL_ICHAR sStr[] = { '%', 'S', 0 };
	static ZIL_ICHAR kStr[] = { '%', 'k', 0 };
	static ZIL_ICHAR pStr[] = { '%', 'p', 0 };

	*string = 0;
	if (jday == 0 && usec < 0)
		// Unanswered value
		return;
	if (!myLocale)
	{
		myLocale = ZIL_LOCALE_MANAGER::UseLocale();
		SetLanguage();
		if (!myLocale)
			return;
	}
	if (!FlagSet(tmFlags, TMF_TWENTY_FOUR_HOUR|TMF_TWELVE_HOUR))
	{
		if (myLocale->data.defaultTimeFlags == -1)
		{
			ZIL_UTIME::Export(string, 128, myLocale->data.timeStringFormat);
			return;
		}
		tmFlags |= myLocale->data.defaultTimeFlags;
	}

	// Determine the seperator character.
	const ZIL_ICHAR *separator = myLocale->data.timeSeparator;
	if (FlagSet(tmFlags, TMF_COLON_SEPARATOR))
		separator = _timeColonSeparator;
	else if (FlagSet(tmFlags, TMF_NO_SEPARATOR))
		separator = _blankString;

	// Determine the time format.
	int needSep = FALSE;
	int needAmPm = FALSE;

	// Format the format string.
	ZIL_ICHAR tmpFormat[64];
	tmpFormat[0] = 0;
	int doZeroFill = FlagSet(tmFlags, TMF_ZERO_FILL);
	if (!FlagSet(tmFlags, TMF_NO_HOURS))
	{
		needSep = TRUE;
		if (FlagSet(tmFlags, TMF_TWENTY_FOUR_HOUR))
		{
			strcat(tmpFormat, (doZeroFill ? hZStr : hStr));
		}
		else if (FlagSet(tmFlags, TMF_TWELVE_HOUR))
		{
			strcat(tmpFormat, (doZeroFill ? iZStr : iStr));
			needAmPm = TRUE;
		}
		else
		{
			strcat(tmpFormat, myLocale->data.timeStringFormat);
			tmFlags = TMF_NO_MINUTES;
		}
	}
	if (!FlagSet(tmFlags, TMF_NO_MINUTES))
	{
		if (needSep)
			strcat(tmpFormat, separator);
		strcat(tmpFormat, mStr);
		needSep = TRUE;
	}
	if (FlagSet(tmFlags, TMF_SECONDS))
	{
		if (needSep)
			strcat(tmpFormat, separator);
		strcat(tmpFormat, sStr);
		needSep = TRUE;
	}
	if (FlagSet(tmFlags, TMF_HUNDREDTHS))
	{
		if (needSep)
			strcat(tmpFormat, myLocale->data.decimalSeparator);
		strcat(tmpFormat, kStr);
		needSep = TRUE;
	}
	if (needAmPm)
	{
		// Append the am/pm string.
		if (needSep)
			strcat(tmpFormat, _timeSpaceString);
		strcat(tmpFormat, pStr);
	}
	ZIL_UTIME::Export(string, 128, tmpFormat);
	// Convert to upper or lower case.
	if (FlagSet(tmFlags, TMF_UPPER_CASE))
		strupr(string);
	else if (FlagSet(tmFlags, TMF_LOWER_CASE))
		strlwr(string);
}

void ZIL_TIME::Export(int *packedTime)
{
	int hour, minute, second;
	
	if (jday == 0 && usec < 0)
		*packedTime = -1;
	else
	{
		ConvertUsec(&hour, &minute, &second, 0);
		*packedTime = (hour << 11) | (minute << 5) | second / 2;
	}
}

TMI_RESULT ZIL_TIME::Import(int hour, int minute, int second, int hundredth)
{
	// Set the time values according to the specified integers.
	if (hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
		second < 0 || second > 59 || hundredth < 0 || hundredth > 99)
		return (TMI_INVALID);
	ZIL_UTIME::Import(0, 0, 1, hour, minute, second, 10 * hundredth);
	return (TMI_OK);
}

TMI_RESULT ZIL_TIME::Import(const ZIL_ICHAR *string, TMF_FLAGS tmFlags)
{
	// Check for a system time.
	if (!string || !string[0])
	{
		if (FlagSet(tmFlags, TMF_SYSTEM))
			return (ZIL_TIME::Import());
		jday = 0;
		usec = -1;
		return (TMI_OK);
	}
	if (!myLocale)
	{
		myLocale = ZIL_LOCALE_MANAGER::UseLocale();
		SetLanguage();
		if (!myLocale)
			return (TMI_INVALID);
	}

	if (!FlagSet(tmFlags, TMF_TWENTY_FOUR_HOUR|TMF_TWELVE_HOUR))
	{
		if (myLocale->data.defaultTimeFlags == -1)
		{
			if (ZIL_UTIME::Import(string, myLocale->data.timeStringFormat) == ZIL_NULLP(ZIL_ICHAR))
				return (TMI_INVALID);
			jday = 0;
			return (TMI_OK);
		}
		tmFlags |= myLocale->data.defaultTimeFlags;
	}

	// Parse out the string time.
	int time[4];
	time[0] = time[1] = time[2] = time[3] = 0;
	static int maxTime[4] = { 23, 59, 59, 99 };
	const ZIL_ICHAR *Separators[4];
	Separators[0] = Separators[1] = myLocale->data.timeSeparator;
	Separators[2] = myLocale->data.decimalSeparator;
	Separators[3] = _blankString;

	// The following breaks if there needs to be non-digit stuff before
	// a time (i.e. am 9:00)
	while (*string && !IsDigit(*string))
		string++;
	if (!string[0])
		return (TMI_INVALID); // An invalid time string was entered.
	int i = 0;
	if (FlagSet(tmFlags, TMF_NO_HOURS))
	{
		i++;
		if (FlagSet(tmFlags, TMF_NO_MINUTES))
			i++;
	}
	for (; i < 4 && *string && IsDigit(*string); i++)
	{
		int j = 0;
		int value = 0;
		while (IsDigit(*string) &&
			(j < 2 || !FlagSet(tmFlags, TMF_NO_SEPARATOR)))
		{
			value = value * 10 + *string - '0';
			if (value > maxTime[i])
				return (TMI_INVALID);
			string++;
			j++;
		}
		if ((i == 0 && !FlagSet(tmFlags, TMF_NO_HOURS)) ||
		    (i == 1 && !FlagSet(tmFlags, TMF_NO_MINUTES)) ||
		    (i == 2 && FlagSet(tmFlags, TMF_SECONDS)) ||
		    (i == 3 && FlagSet(tmFlags, TMF_HUNDREDTHS)))
			time[i] = value;
		if (!FlagSet(tmFlags, TMF_NO_SEPARATOR))
		{
			const ZIL_ICHAR *separator = Separators[i];
			if (*string == ':')
				string++;
			else while (*string && *string == *separator)
				string++, separator++;
		}
	}

	// Find the am/pm string (if any).
	while (*string && !IsAlpha(*string))
		string++;
	if (*string && IsAlpha(*string))
	{
		if (strnicmp(string, myTimeStrings->GetMessage(ZIL_PM), 1) == 0)
		{
			if (time[0] > 12)
				return (TMI_INVALID);
			if (time[0] < 12)
				time[0] += 12;
		}
		else if (strnicmp(string, myTimeStrings->GetMessage(ZIL_AM), 1) == 0)
		{
			if (time[0] > 12)
				return (TMI_INVALID);
			if (time[0] == 12)
				time[0] -= 12;
		}
		else
			return (TMI_INVALID); // An invalid time string was entered.
	}

	// Set the time according to the parsed values.
	return (Import(time[0], time[1], time[2], time[3]));
}

TMI_RESULT ZIL_TIME::Import(int packedTime)
{
	if (packedTime == -1)
	{
		jday = 0;
		usec = -1;		// This can work only because -1 isn't
		return (TMI_OK);	// a valid packed time.
	}
	ZIL_UTIME::Import(0, 0, 1, (packedTime & 0xF800) >> 11, (packedTime & 0x07E0) >> 5, 2 * (packedTime & 0x001F), 0);
	return (TMI_OK);
}

