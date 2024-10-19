//	Zinc Interface Library - Z_UTIME.CPP
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


// Comment the next line if the system doesn't define gettimeofday().
#define ZIL_GET_TIME

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
#if !defined(ZIL_GET_TIME) || defined(SCO_UNIX)
	extern "C" int gettimeofday(struct timeval *tp, struct timezone *tzp);
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

// We don't really support leap seconds
const ZIL_INT32 usecsPerDay = (24L * 60L * 60L * 1000L);

#define ZIL_UTIME_DATA
#include "lang_def.cpp"

ZIL_UTIME::ZIL_UTIME(void) :
	jday(0), usec(0), recurse(FALSE)
{
	myLocale = ZIL_LOCALE_MANAGER::UseLocale();
	SetLanguage();
	Import();
}

ZIL_UTIME::ZIL_UTIME(const ZIL_UTIME &utime) :
	jday(utime.jday), usec(utime.usec), recurse(FALSE)
{
	myLocale = ZIL_LOCALE_MANAGER::UseLocale();
	SetLanguage();
}

ZIL_UTIME::ZIL_UTIME(int year, int month, int day,
		int hour, int minute, int second, int milliSecond) :
	jday(0), usec(0), recurse(FALSE)
{
	myLocale = ZIL_LOCALE_MANAGER::UseLocale();
	SetLanguage();
	Import(year, month, day, hour, minute, second, milliSecond);
}

ZIL_UTIME::~ZIL_UTIME(void)
{
	ZIL_LOCALE_MANAGER::FreeLocale(myLocale);
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myDayStrings);
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myMonthStrings);
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myTimeStrings);
}

void ZIL_UTIME::Export(int *year, int *month, int *day,
	int *hour, int *minute, int *second, int *milliSecond)
{
	ConvertJday(year, month, day, ZIL_NULLP(int));
	ConvertUsec(hour, minute, second, milliSecond);
}

ZIL_UTIME::UTMI_RESULT ZIL_UTIME::Import(void)
{
	// Set the time values according to the system time.
#if defined(ZIL_POSIX)
#define USE_GETTIMEOFDAY		// Must use this for usec stuff.
#	if defined(USE_GETTIMEOFDAY)
	// Use GREAT Berkeley function!!!
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	time_t t;
	time(&t);
	struct tm *localTime = localtime(&t);	// Used for consistent DST info.
	Import(1900+localTime->tm_year, localTime->tm_mon+1, localTime->tm_mday,
		localTime->tm_hour, localTime->tm_min, localTime->tm_sec, (int)(tv.tv_usec / 1000));
#	else
	// Do boring POSIX stuff...
	time_t t;
	time(&t);
	struct tm *localTime = localtime(&t);
	Import(1900+localTime->tm_year, localTime->tm_mon+1, localTime->tm_mday,
		localTime->tm_hour, localTime->tm_min, localTime->tm_sec, 0);
#	endif
#elif defined(ZIL_WINNT)
	time_t t;
	time(&t);		// This function MUST be called before GetSystemTime().
	struct tm *localTime = localtime(&t);
	SYSTEMTIME gmtTime;
	GetSystemTime(&gmtTime);

	Import(1900+localTime->tm_year, localTime->tm_mon+1, localTime->tm_mday,
		localTime->tm_hour, localTime->tm_min, localTime->tm_sec,
		(localTime->tm_sec == gmtTime.wSecond) ?
		gmtTime.wMilliseconds : 0);
#elif defined(ZIL_MACINTOSH)
	DateTimeRec t;
	GetTime(&t);
	Import(t.year, t.month, t.day, t.hour, t.minute, t.second, 0);
#elif defined(__ZTC__)
	struct dos_time_t tInfo;
	dos_gettime(&tInfo);
	struct dos_date_t dInfo;
	dos_getdate(&dInfo);
	Import(dInfo.year, dInfo.month, dInfo.day,
		tInfo.hour, tInfo.minute, tInfo.second, tInfo.hsecond * 10);
#elif defined(_MSC_VER)
	struct _dostime_t tInfo;
	_dos_gettime(&tInfo);
	struct _dosdate_t dInfo;
	_dos_getdate(&dInfo);
	Import(dInfo.year, dInfo.month, dInfo.day,
		tInfo.hour, tInfo.minute, tInfo.second, tInfo.hsecond * 10);
#elif defined(__WATCOMC__) || defined(__HIGHC__)
	struct dostime_t tInfo;
	_dos_gettime(&tInfo);
	struct dosdate_t dInfo;
	_dos_getdate(&dInfo);
	Import(dInfo.year, dInfo.month, dInfo.day,
		tInfo.hour, tInfo.minute, tInfo.second, tInfo.hsecond * 10);
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	struct time tinfo;
	gettime(&tinfo);
	struct date dinfo;
	getdate(&dinfo);
	Import(dinfo.da_year, dinfo.da_mon, dinfo.da_day,
		tinfo.ti_hour, tinfo.ti_min, tinfo.ti_sec, tinfo.ti_hund * 10);
#elif defined(__IBMCPP__) || defined(__GLOCKENSPIEL__)
	DATETIME info;
	DosGetDateTime(&info);
	Import(info.year, info.month, info.day,
		info.hours, info.minutes, info.seconds, info.hundredths * 10);
#elif defined(__GNUC__) && defined(__DVX__)
	time_t t;
	time(&t);
	struct tm *localTime = localtime(&t);
	Import(1900+localTime->tm_year, localTime->tm_mon+1, localTime->tm_mday,
		localTime->tm_hour, localTime->tm_min, localTime->tm_sec, 0);
#endif
	return (UTMI_OK);
}

ZIL_UTIME::UTMI_RESULT ZIL_UTIME::Import(const ZIL_UTIME &utime)
{
	jday = utime.jday;
	usec = utime.usec;
	myLocale = ZIL_LOCALE_MANAGER::UseLocale(utime.myLocale);
	myDayStrings   = ZIL_LANGUAGE_MANAGER::UseLanguage(utime.myDayStrings);
	myMonthStrings = ZIL_LANGUAGE_MANAGER::UseLanguage(utime.myMonthStrings);
	myTimeStrings  = ZIL_LANGUAGE_MANAGER::UseLanguage(utime.myTimeStrings);
	return (UTMI_OK);
}

int ZIL_UTIME::LeapYear(void)
{
	int year;
	ConvertJday(&year, ZIL_NULLP(int), ZIL_NULLP(int), ZIL_NULLP(int));
	return ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)));
}

ZIL_UTIME *ZIL_UTIME::operator=(const ZIL_UTIME &utime)
{
	Import(utime);
	return (this);
}

ZIL_UTIME *ZIL_UTIME::operator+(const ZIL_UTIME &utime)
{
	jday += utime.jday;
	usec += utime.usec;
	MakeCanonical();
	return (this);
}

ZIL_UTIME *ZIL_UTIME::operator-(const ZIL_UTIME &utime)
{
	jday -= utime.jday;
	usec -= utime.usec;
	MakeCanonical();
	return (this);
}

int ZIL_UTIME::operator==(const ZIL_UTIME &utime)
{
	return (jday == utime.jday && usec == utime.usec);
}

int ZIL_UTIME::operator!=(const ZIL_UTIME &utime)
{
	return (jday != utime.jday || usec != utime.usec);
}

int ZIL_UTIME::operator>(const ZIL_UTIME &utime)
{
	return (jday > utime.jday ||
		jday == utime.jday && usec > utime.usec);
}

int ZIL_UTIME::operator>=(const ZIL_UTIME &utime)
{
	return !(jday < utime.jday ||
		 jday == utime.jday && usec < utime.usec);
}

int ZIL_UTIME::operator<(const ZIL_UTIME &utime)
{
	return (jday < utime.jday ||
		jday == utime.jday && usec < utime.usec);
}

int ZIL_UTIME::operator<=(const ZIL_UTIME &utime)
{
	return !(jday > utime.jday ||
		 jday == utime.jday && usec > utime.usec);
}

void ZIL_UTIME::SetLanguage(const ZIL_ICHAR *languageName)
{
	if (!defaultInitialized)
	{
		ZIL_LANGUAGE_MANAGER::SetLanguage(_dayName, DAY_STRINGS);
		ZIL_LANGUAGE_MANAGER::SetLanguage(_monthName, MONTH_STRINGS);
		ZIL_LANGUAGE_MANAGER::SetLanguage(_timeName, TIME_STRINGS);
		defaultInitialized = TRUE;
	}
	myDayStrings   = ZIL_LANGUAGE_MANAGER::UseLanguage(_dayName, languageName);
	myMonthStrings = ZIL_LANGUAGE_MANAGER::UseLanguage(_monthName, languageName);
	myTimeStrings  = ZIL_LANGUAGE_MANAGER::UseLanguage(_timeName, languageName);
	if (!myDayStrings)			// patch
		defaultInitialized = FALSE;	// patch
}

void ZIL_UTIME::ConvertUsec(int *hour, int *minute, int *second, int *milliSecond)
{
	// usec < 0 means "blank"
	ZIL_UINT32 tmp = (usec < 0 ? 0 : usec);
	// Set the integer variables.
	if (hour)
		*hour = (int)(tmp / 3600000L);
	tmp %= 3600000L;
	if (minute)
		*minute = (int)(tmp / 60000L);
	tmp %= 60000L;
	if (second)
		*second = (int)(tmp / 1000L);
	tmp %= 1000L;
	if (milliSecond)
		*milliSecond = (int)tmp;
}

int ZIL_UTIME::DayOfWeek(void)
{
	return (int)((jday + 1) % 7);
}

void ZIL_UTIME::ConvertJday(int *pYear, int *pMonth, int *pDay, int *pDayOfWeek)
{
	long year = 0, month = 0, day = 0;
	// Jday == 0 and usec < 0 is a "blank".
	if (jday != 0 || usec >= 0)
	{
		// Compute the separate date values algorithm 5 of Practical
		// Astronomy with your Calculator.  This includes a fix for the
		// Gregorian calendar change in 1582.  Protestant countries should
		// change that value to when they adopted the Gregorian calendar.
		long C = jday;
		if (C > myLocale->data.beginGregorian)
		{
			long A = (100 * C - 186721625L) / 3652425L;
			C += 1 + A - A / 4;
		}
		C += 1524;
		long D = (100 * C - 12210L) / 36525L;
		long E = (1461L * D) / 4;
		long G = ((C - E) * 10000) / 306001L;
		day   = C - E - (G * 306001L) / 10000;
		month = G - ((G > 13) ? 13 : 1);
		year  = D - ((month > 2) ? 4716 : 4715);
		if (year <= 0)		// There is no year 0 (1 BC = -1).
			year--;
		// Set the integer variables.
	}
	if (pYear)
		*pYear = (int)year;
	if (pMonth)
		*pMonth = (int)month;
 	if (pDay)
		*pDay = (int)day;
	if (pDayOfWeek)
		*pDayOfWeek = DayOfWeek();
}

void ZIL_UTIME::Import(int year, int month, int day, int hour, int minute, int second, int millisecond)
{
	if ((year | month) == 0)
		jday = day;
	else
	{
		if (year < 0)
			year++;
		// Compute the Julian date from three integer values using
		// algorithm 4 of Practical Astronomy with your Calculator.
		// This include a fix for the Gregorian calendar change in
		// 1582.  Protestant countries should change that value to
		// when they adopted the Gregorian calendar.
		if (month <= 2)
		{
			month += 12;
			year--;
		}
		jday = 365L * year + (153L * (month + 1)) / 5 + day + 1720995L;
		jday += ((year >= 0) ? year : (year-3)) / 4;
		// Gregorian date fix
		if ((myLocale && jday > myLocale->data.beginGregorian + myLocale->data.skipGregorian) ||
		    (!myLocale && jday > 2299160L + 11))
//		if (jday > myLocale->data.beginGregorian + myLocale->data.skipGregorian)
			jday += 2 - (year / 100) + ((year / 100) / 4);
	}
	usec = millisecond + 1000L * (second + 60L * (minute + 60L * hour));

	MakeCanonical();
	// If all zeros are sent in, mark this as "blank".
	if (jday == 0 && usec == 0)
		usec = -1;
}

void ZIL_UTIME::MakeCanonical(void)
{
	if (usec >= usecsPerDay)
	{
		jday += usec / usecsPerDay;
		usec %= usecsPerDay;
	}
	else
		while (usec < 0)
		{
			jday--;
			usec += usecsPerDay;
		}
}

ZIL_INT32 ZIL_UTIME::Difference(const ZIL_UTIME &utime)
{
	ZIL_INT32 _jday = jday - utime.jday;
	ZIL_INT32 _usec = usec - utime.usec;
	return (_jday * usecsPerDay + _usec) / 10;
}
