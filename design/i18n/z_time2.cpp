//	Zinc Interface Library - Z_TIME2.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_LOC_TIME
#include "p_i18n.hpp"

// ----- LOC_TIME -----------------------------------------------------------

void ZAF_LOCALE_WINDOW::LOC_TIME_DataLoad(void)
{
	// Load the object data.
//	SetText(FORMAT, table->decimalSeparator);
//	SetText(TWELVE_HOUR, table->decimalSeparator);
//	SetText(TWENTY_FOUR_HOUR, table->decimalSeparator);
	SetText(SEPARATOR, table->timeSeparator);
	SetText(TIME_FORMAT, table->timeStringFormat);
	SetText(TIME_12_FORMAT, table->time12StringFormat);

	// Load the object flags.
	// format
	DTF_FLAGS tmFlags = table->defaultTimeFlags;
	SetFlag(FLAG_HUNDREDTHS, tmFlags, TMF_HUNDREDTHS);
	SetFlag(FLAG_SECONDS, tmFlags, TMF_SECONDS);
	SetFlag(FLAG_NO_MINUTES, tmFlags, TMF_NO_MINUTES);
	SetFlag(FLAG_NO_HOURS, tmFlags, TMF_NO_HOURS);
	SetFlag(FLAG_LOWER_CASE, tmFlags, TMF_LOWER_CASE);
	SetFlag(FLAG_UPPER_CASE, tmFlags, TMF_UPPER_CASE);
	SetFlag(FLAG_ZERO_FILL, tmFlags, TMF_ZERO_FILL);
	SetFlag(FLAG_SYSTEM, tmFlags, TMF_SYSTEM);
	// separators
	SetFlag(FLAG_DEFAULT_SEPARATOR,
		FlagSet(tmFlags, TMF_COLON_SEPARATOR | TMF_NO_SEPARATOR), 0);
	SetFlag(FLAG_NO_SEPARATOR, tmFlags, TMF_NO_SEPARATOR);
	SetFlag(FLAG_COLON_SEPARATOR, tmFlags, TMF_COLON_SEPARATOR);
	// country
	SetFlag(FLAG_12_HOUR, tmFlags, TMF_TWELVE_HOUR);
	SetFlag(FLAG_24_HOUR, tmFlags, TMF_TWENTY_FOUR_HOUR);
}

void ZAF_LOCALE_WINDOW::LOC_TIME_DataStore(void)
{
	// Store the object data.
//	GetText(FORMAT, table->decimalSeparator);
//	GetText(TWELVE_HOUR, table->decimalSeparator);
//	GetText(TWENTY_FOUR_HOUR, table->decimalSeparator);
	GetText(SEPARATOR, table->timeSeparator);
	GetText(TIME_FORMAT, &table->timeStringFormat);
	GetText(TIME_12_FORMAT, &table->time12StringFormat);

	// Store the object flags.
	// format
	DTF_FLAGS tmFlags = 0;
	GetFlag(FLAG_HUNDREDTHS, &tmFlags, TMF_HUNDREDTHS);
	GetFlag(FLAG_SECONDS, &tmFlags, TMF_SECONDS);
	GetFlag(FLAG_NO_MINUTES, &tmFlags, TMF_NO_MINUTES);
	GetFlag(FLAG_NO_HOURS, &tmFlags, TMF_NO_HOURS);
	GetFlag(FLAG_LOWER_CASE, &tmFlags, TMF_LOWER_CASE);
	GetFlag(FLAG_UPPER_CASE, &tmFlags, TMF_UPPER_CASE);
	GetFlag(FLAG_ZERO_FILL, &tmFlags, TMF_ZERO_FILL);
	GetFlag(FLAG_SYSTEM, &tmFlags, TMF_SYSTEM);
	// separators
	GetFlag(FLAG_NO_SEPARATOR, &tmFlags, TMF_NO_SEPARATOR);
	GetFlag(FLAG_COLON_SEPARATOR, &tmFlags, TMF_COLON_SEPARATOR);
	// country
	GetFlag(FLAG_12_HOUR, &tmFlags, TMF_TWELVE_HOUR);
	GetFlag(FLAG_24_HOUR, &tmFlags, TMF_TWENTY_FOUR_HOUR);
	table->defaultTimeFlags = tmFlags;
}

