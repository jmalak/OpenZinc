//	Zinc Interface Library - Z_UTIME1.CPP
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


#define INCL_DOSDATETIME
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
extern "C"
{
#endif
#	include <stdio.h>
#	include <stdlib.h>
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
}
#endif
#include "ui_gen.hpp"
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || (defined(ZIL_OS2) && !defined(__IBMCPP__) && !defined(__GLOCKENSPIEL__)) || defined(__DVX__)
#	include <dos.h>
#endif
#if defined(ZIL_MSWINDOWS) || defined(ZIL_POSIX)
extern "C"
{
#	include <time.h>
}
#endif
#if defined(ZIL_POSIX)
	extern "C"
	{
#	include <sys/time.h>
	}
#endif
#if defined(_MSC_VER) && defined(DOS386)
#	include <pldos32.h>
#endif

static int _daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static ZIL_ICHAR _dFormat[] = { '%', 'm', '/', '%', 'd', '/', '%', 'y', 0 };
static ZIL_ICHAR _rFormat[] = { '%', 'H', ':', '%', 'M', 0 };
static ZIL_ICHAR _tFormat[] = { '%', 'H', ':', '%', 'M', ':', '%', 'S', 0 };

static struct {
	ZIL_ICHAR *name;
	int offset;
} _timeZones[] = {
	{ ZIL_NULLP(ZIL_ICHAR), 0 }
};

ZIL_UTIME::ZIL_UTIME(const ZIL_ICHAR *string) :
	jday(0), usec(0), recurse(FALSE)
{
	myLocale = ZIL_LOCALE_MANAGER::UseLocale();
	SetLanguage();
	Import(string, ZIL_NULLP(ZIL_ICHAR));
}

int ZIL_UTIME::DaysInMonth(int month)
{
	if (month == 0)
		ConvertJday(ZIL_NULLP(int), &month, ZIL_NULLP(int), ZIL_NULLP(int));
	if (month == 2 && LeapYear())
		return (29);
	return (month ? _daysInMonth[month - 1] : 0);
}

int ZIL_UTIME::Export(ZIL_ICHAR *string, int maxsize, const ZIL_ICHAR *format)
{
	if (jday == 0 && usec < 0)
	{
		*string = 0;
		return (UTMI_OK);
	}
	if (!myLocale)
	{
		myLocale = ZIL_LOCALE_MANAGER::UseLocale();
		SetLanguage();
		if (!myLocale)
			return (UTMI_INVALID);
	}
	int currentMaxsize = maxsize;

	int hour, minute, second, milliSecond;
	ConvertUsec(&hour, &minute, &second, &milliSecond);

	int pYear, pMonth, pDay, pDayOfWeek;
	ConvertJday(&pYear, &pMonth, &pDay, &pDayOfWeek);

	int pDayOfYear = 0;
	for (int j=0; j < pMonth-1; j++)
		pDayOfYear += _daysInMonth[j];
	pDayOfYear += pDay;
	if (LeapYear() && pMonth > 2)
		pDayOfYear++;

	ZIL_UTIME firstDay(pYear, 1, 1, 0, 0, 0, 0);
	int firstDayOfYear = firstDay.DayOfWeek();

	ZIL_ICHAR *ECval = ZIL_NULLP(ZIL_ICHAR), *EYval = ZIL_NULLP(ZIL_ICHAR);
	int Eyval, i = 0;
	for (; i < myLocale->data.eraTableLen; i++)
	{
		if (myLocale->data.eraTable[i].startDate <= jday && jday <= myLocale->data.eraTable[i].endDate)
		{
			ECval = myLocale->data.eraTable[i].eraName;
			EYval = myLocale->data.eraTable[i].eraFormat;
			ZIL_UTIME tmp; tmp.jday = myLocale->data.eraTable[i].startDate;
			int baseYear;
			tmp.ConvertJday(&baseYear, ZIL_NULLP(int), ZIL_NULLP(int), ZIL_NULLP(int));
			if (myLocale->data.eraTable[i].direction)
				Eyval = myLocale->data.eraTable[i].offset + pYear - baseYear;
			else
				Eyval = myLocale->data.eraTable[i].offset + baseYear - pYear;
		}
	}

	for (i=0; format[i]; i++)
	{
		int doCopy = TRUE;
		ZIL_ICHAR tmpStr[40];
		const ZIL_ICHAR *copyStr = tmpStr;
		if (format[i] == '%')
		{
			i++;
			if (format[i] == 0)
				break;
			switch (format[i])
			{
			case 'a':
				copyStr = myDayStrings->GetMessage(ZIL_ABDAY_1+pDayOfWeek);
				break;
			case 'A':
				copyStr = myDayStrings->GetMessage(ZIL_DAY_1+pDayOfWeek);
				break;
			case 'b':
				copyStr = myMonthStrings->GetMessage(ZIL_ABMON_1+pMonth-1);
				break;
			case 'B':
				copyStr = myMonthStrings->GetMessage(ZIL_MON_1+pMonth-1);
				break;
			case 'c':
				maxsize -= Export(string, maxsize, myLocale->data.dateTimeStringFormat);
				doCopy = FALSE;
				break;
			case 'C':
				itoa(pYear / 100, tmpStr, 10, 2);
				break;
			case 'd':
				itoa(pDay, tmpStr, 10, 2);
				break;
			case 'D':
				maxsize -= Export(string, maxsize, _dFormat);
				doCopy = FALSE;
				break;
			case 'e':
				itoa(pDay, tmpStr, 10, 0);
				break;
			case 'E':	// Era conversions
				i++;
				switch (format[i])
				{
				case 'C':
					if (ECval)
						copyStr = ECval;
					else
						itoa(pYear / 100, tmpStr, 10, 2);
					break;
				case 'y':
					if (ECval)
						itoa(Eyval, tmpStr, 10, 0);
					else
						itoa(pYear % 100, tmpStr, 10, 2);
					break;
				case 'Y':
					maxsize -= Export(string, maxsize, EYval);
					doCopy = FALSE;
					break;
				}
				break;
			case 'g':
				copyStr = myMonthStrings->GetMessage(ZIL_SHMON_1+pMonth-1);
				break;
			case 'G':
				copyStr = myDayStrings->GetMessage(ZIL_SHDAY_1+pDayOfWeek);
				break;
			case 'h':
				itoa(hour, tmpStr, 10, 0);
				break;
			case 'H':
				itoa(hour, tmpStr, 10, 2);
				break;
			case 'i':
			{
				int tmp = hour % 12;
				itoa((tmp == 0 ? 12 : tmp), tmpStr, 10, 0);
				break;
			}
			case 'I':
			{
				int tmp = hour % 12;
				itoa((tmp == 0 ? 12 : tmp), tmpStr, 10, 2);
				break;
			}
			case 'j':
				itoa(pDayOfYear, tmpStr, 10, 3);
				break;
			case 'J':
				itoa(milliSecond / 100, tmpStr, 10, 1);
				break;
			case 'k':
				itoa(milliSecond / 10, tmpStr, 10, 2);
				break;
			case 'K':
				itoa(milliSecond, tmpStr, 10, 3);
				break;
			case 'l':
			case 'L':
				i++;
				switch (format[i])
				{
				case 'm':
					itoa(12*pYear + pMonth, tmpStr, 10);
					break;
				case 'd':
					itoa(jday, tmpStr, 10);
					break;
				case 'H':
					itoa(24 * jday + usec / (1000*60*60), tmpStr, 10);
					break;
				case 'M':
					itoa(24 * 60 * jday + usec / (1000*60), tmpStr, 10);
					break;
				case 'S':
					itoa(24 * 60 * 60 * jday + usec / (1000), tmpStr, 10);
					break;
				case 0:
					i--;
					break;
				}
				break;
			case 'm':
				itoa(pMonth, tmpStr, 10, 2);
				break;
			case 'M':
				itoa(minute, tmpStr, 10, 2);
				break;
			case 'n':
				tmpStr[0] = '\n';
				tmpStr[1] = 0;
				break;
			// case 'O':	alternate digits
			case 'p':
				copyStr = myTimeStrings->GetMessage(hour < 12 ? ZIL_AM : ZIL_PM);
				break;
			case 'r':
				maxsize -= Export(string, maxsize, myLocale->data.time12StringFormat);
				doCopy = FALSE;
				break;
			case 'R':
				maxsize -= Export(string, maxsize, _rFormat);
				doCopy = FALSE;
				break;
			case 's':
				itoa(second, tmpStr, 10, 0);
				break;
			case 'S':
				itoa(second, tmpStr, 10, 2);
				break;
			case 't':
				tmpStr[0] = '\t';
				tmpStr[1] = 0;
				break;
			case 'T':
				maxsize -= Export(string, maxsize, _tFormat);
				doCopy = FALSE;
				break;
			case 'u':
				itoa((pDayOfWeek == 0 ? 7 : pDayOfWeek), tmpStr, 10, 0);
				break;
			case 'U':
			{
				int bias = (firstDayOfYear <= 0 ? 6 : firstDayOfYear - 1);
				itoa((pDayOfYear+bias)/7, tmpStr, 10, 2);
				break;
			}
			case 'v':
				itoa(pMonth, tmpStr, 10, 0);
				break;
			case 'V':
				itoa(minute, tmpStr, 10, 0);
				break;
			case 'w':
				itoa(pDayOfWeek, tmpStr, 10, 0);
				break;
			case 'W':
			{
				int bias = (firstDayOfYear <= 1 ? 5 + firstDayOfYear : firstDayOfYear - 2);
				itoa((pDayOfYear+bias)/7, tmpStr, 10, 2);
				break;
			}
			case 'x':
				maxsize -= Export(string, maxsize, myLocale->data.dateStringFormat);
				doCopy = FALSE;
				break;
			case 'X':
				maxsize -= Export(string, maxsize, myLocale->data.timeStringFormat);
				doCopy = FALSE;
				break;
			case 'y':
				itoa(pYear % 100, tmpStr, 10, 2);
				break;
			case 'Y':
				itoa(pYear, tmpStr, 10, 0);
				break;
			case 'Z':
				copyStr = myTimeStrings->GetMessage(ZIL_TZ);
				break;
			case '%':
				tmpStr[0] = '%';
				tmpStr[1] = 0;
				break;
			default:
				tmpStr[0] = 0;
				break;
			}
		}
		else
		{
			tmpStr[0] = format[i];
			tmpStr[1] = 0;
		}
		if (doCopy)
		{
			maxsize -= strlen(copyStr);
			if (maxsize <= 0)
				return (UTMI_OK);
			strcpy(string, copyStr);
		}
		string += strlen(string);
	}
	*string++ = format[i];
	return (currentMaxsize-maxsize);
}

int ZIL_UTIME::Export(ZIL_ICHAR *string, int maxsize, const ZIL_ICHAR **formats)
{
	for (int i=0; formats[i]; i++)
	{
		int j = Export(string, maxsize, formats[i]);
		if (j != UTMI_OK)
			return (j);
	}
	return (UTMI_OK);
}

const ZIL_ICHAR *ZIL_UTIME::Import(const ZIL_ICHAR *string, const ZIL_ICHAR *format)
{
	if (*string == 0)
	{
		jday = 0;
		usec = -1;
		return (string);
	}
	if (!myLocale)
	{
		myLocale = ZIL_LOCALE_MANAGER::UseLocale();
		SetLanguage();
		if (!myLocale)
			return (string);
	}
	if (!format)
		format = myLocale->data.dateTimeStringFormat;
	if (recurse)
		jday = usec = 0;
	int saveRecurse = recurse;
	int hour, minute, second, milliSecond;
	ConvertUsec(&hour, &minute, &second, &milliSecond);

	int pYear, pMonth, pDay, pDayOfWeek;
	ConvertJday(&pYear, &pMonth, &pDay, &pDayOfWeek);

	int pDayOfYear = 0;
	int pWeek = 0;
	int timeOffset = 0;
	for (int i=0; format[i]; i++)
	{
		ZIL_ICHAR *next = (ZIL_ICHAR *)string;
		if (IsSpace(format[i]))
		{
			while (IsSpace(*next))
			       next++;
		}
		else if (format[i] == '%')
		{
			i++;
			if (format[i] == 0)
				return (ZIL_NULLP(ZIL_ICHAR));
			switch (format[i])
			{
			case 'a':
			case 'A':
			{
				int count = 0;
				for (int j = 0; j < 7; j++)
				{
					const ZIL_ICHAR *tmp = myDayStrings->GetMessage(j+ZIL_ABDAY_1);
					if (strnicmp(string, tmp, strlen(tmp)) == 0)
					{
						count++;
						pDayOfWeek = j;
						next += strlen(tmp);
						continue;
					}
					tmp = myDayStrings->GetMessage(j+ZIL_DAY_1);
					if (strnicmp(string, tmp, strlen(tmp)) == 0)
					{
						count++;
						pDayOfWeek = j;
						next += strlen(tmp);
						continue;
					}
				}
				if (count != 1)
					return (ZIL_NULLP(ZIL_ICHAR));
				break;
			}
			case 'h':
			case 'b':
			case 'B':
			{
				int count = 0;
				for (int j = 0; j < 12; j++)
				{
					const ZIL_ICHAR *tmp = myMonthStrings->GetMessage(j+ZIL_ABMON_1);
					if (strnicmp(string, tmp, strlen(tmp)) == 0)
					{
						count++;
						pMonth = j + 1;
						next += strlen(tmp);
						continue;
					}
					tmp = myMonthStrings->GetMessage(j+ZIL_MON_1);
					if (strnicmp(string, tmp, strlen(tmp)) == 0)
					{
						count++;
						pMonth = j + 1;
						next += strlen(tmp);
						continue;
					}
				}
				if (count != 1)
					return (ZIL_NULLP(ZIL_ICHAR));
				break;
			}
			case 'c':
				recurse = TRUE;
				next = (ZIL_ICHAR *)Import(string, myLocale->data.dateTimeStringFormat);
				break;
			case 'C': // This doesn't match the XOpen guide
				pYear = (int)strtol(string, &next, 10);
				pYear *= 100;
				break;
			case 'd':
				pDay = (int)strtol(string, &next, 10);
				if (pDay < 1 || pDay > 31)
					return (ZIL_NULLP(ZIL_ICHAR));
				break;
			case 'D':
				recurse = TRUE;
				next = (ZIL_ICHAR *)Import(string, _dFormat);
				break;
			// case 'e':	see 'd'
			// case 'E':	// Era conversions
			// case 'h':	see 'b'
			case 'H':
				hour = (int)strtol(string, &next, 10);
				if (hour < 0 || hour > 23)
					return (ZIL_NULLP(ZIL_ICHAR));
				break;
			case 'I':
				hour = (int)strtol(string, &next, 10);
				if (hour < 1 || hour > 12)
					return (ZIL_NULLP(ZIL_ICHAR));
				hour--;
				break;
			case 'j':
				pDayOfYear = (int)strtol(string, &next, 10);
				if (pDayOfYear < 1 || pDayOfYear > 366)
					return (ZIL_NULLP(ZIL_ICHAR));
				pDayOfYear--;
				break;
			case 'm':
				pMonth = (int)strtol(string, &next, 10);
				if (pMonth < 1 || pMonth > 12)
					return (ZIL_NULLP(ZIL_ICHAR));
				break;
			case 'M':
				minute = (int)strtol(string, &next, 10);
				if (minute < 0 || minute > 59)
					return (ZIL_NULLP(ZIL_ICHAR));
				string = next - 1;
				break;
			case 't':
			case 'n':
				while (IsSpace(*next))
				       next++;
				break;
			// case 'O':	????
			case 'p':
				{
				const ZIL_ICHAR *tmp = myTimeStrings->GetMessage(ZIL_AM);
				if (strnicmp(string, tmp, strlen(tmp)) == 0)
				{
					next += strlen(tmp);
					break;
				}
				tmp = myTimeStrings->GetMessage(ZIL_PM);
				if (strnicmp(string, tmp, strlen(tmp)) == 0)
				{
					timeOffset = 12 * 60 * 60 * 1000;
					next += strlen(tmp);
					break;
				}
				return (ZIL_NULLP(ZIL_ICHAR));
				}
			case 'r': // This doesn't match the XOpen guide
				recurse = TRUE;
				next = (ZIL_ICHAR *)Import(string, myLocale->data.time12StringFormat);
				break;
			case 'R':
				recurse = TRUE;
				next = (ZIL_ICHAR *)Import(string, _rFormat);
				break;
			case 'S':
				second = (int)strtol(string, &next, 10);
				if (second < 0 || second > 61)
					return (ZIL_NULLP(ZIL_ICHAR));
				break;
			// case 't':	see 'n'
			case 'T':
				recurse = TRUE;
				next = (ZIL_ICHAR *)Import(string, _tFormat);
				break;
			case 'U':
				pWeek = (int)strtol(string, &next, 10);
				if (pWeek < 0 || pWeek > 55)
					return (ZIL_NULLP(ZIL_ICHAR));
				break;
			case 'w':
				pDayOfWeek = (int)strtol(string, &next, 10);
				if (pDayOfWeek < 0 || pDayOfWeek > 6)
					return (ZIL_NULLP(ZIL_ICHAR));
				break;
			case 'W':
				pDayOfWeek = (int)strtol(string, &next, 10);
				if (pDayOfWeek < 1 || pDayOfWeek > 7)
					return (ZIL_NULLP(ZIL_ICHAR));
				if (pDayOfWeek == 7)
					pDayOfWeek = 0;
				break;
			case 'x':
				recurse = TRUE;
				next = (ZIL_ICHAR *)Import(string, myLocale->data.dateStringFormat);
				break;
			case 'X':
				recurse = TRUE;
				next = (ZIL_ICHAR *)Import(string, myLocale->data.timeStringFormat);
				break;
			case 'y':
			{
				basisYear = pYear - (pYear % 100);
				pYear = (int)strtol(string, &next, 10);
				if (pYear < 0 || pYear > 99)
					return (ZIL_NULLP(ZIL_ICHAR));
				pYear += basisYear;
				break;
			}
			case 'Y':
				pYear = (int)strtol(string, &next, 10);
				break;
			case 'Z':
			{
				for (int j = 0; _timeZones[j].name; j++)
					if (strnicmp(string, _timeZones[j].name, strlen(_timeZones[j].name)) == 0)
					{
						zoneOffset = _timeZones[j].offset;
						next += strlen(_timeZones[j].name);
						break;
					}
				break;
			}
			case '%':
				if (*next != '%')
					return (ZIL_NULLP(ZIL_ICHAR));
				next++;
				break;
			default:
				break;
			}
		}
		else
		{
			if (format[i] != *next)
				return (ZIL_NULLP(ZIL_ICHAR));
			next++;
		}
		recurse = saveRecurse;
		if (next == ZIL_NULLP(ZIL_ICHAR))
			return (ZIL_NULLP(ZIL_ICHAR));
		string = next;
	}	
	Import(pYear, pMonth, pDay, hour, minute - timeOffset, second, 0);
	return (string);
}

