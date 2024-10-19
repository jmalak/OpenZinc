//	Zinc Interface Library - Z_DATE1.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_LOC_DATE
#include "p_i18n.hpp"

// ----- LOC_DATE -----------------------------------------------------------

void ZAF_LOCALE_WINDOW::LOC_DATE_DataLoad(void)
{
	// Load the object data.
//	SetText(FORMAT, table->data.decimalSeparator);
	SetText(SEPARATOR, table->dateSeparator);
	SetText(DATE_STRING_FORMAT, table->dateStringFormat);
	SetText(DATE_TIME_STRING_FORMAT, table->dateTimeStringFormat);

	// Load the default flags.
	// format
	DTF_FLAGS dtFlags = table->defaultDateFlags;
	SetFlag(FLAG_SHORT_DAY, dtFlags, DTF_SHORT_DAY);
	SetFlag(FLAG_DAY_OF_WEEK, dtFlags, DTF_DAY_OF_WEEK);
	SetFlag(FLAG_SHORT_MONTH, dtFlags, DTF_SHORT_MONTH);
	SetFlag(FLAG_MONTH, dtFlags, DTF_ALPHA_MONTH);
	SetFlag(FLAG_SHORT_YEAR, dtFlags, DTF_SHORT_YEAR);
	SetFlag(FLAG_UPPER_CASE, dtFlags, DTF_UPPER_CASE);
	SetFlag(FLAG_ZERO_FILL, dtFlags, DTF_ZERO_FILL);
	SetFlag(FLAG_SYSTEM, dtFlags, DTF_SYSTEM);
	// separators
	SetFlag(FLAG_DEFAULT_SEPARATOR,
		FlagSet(dtFlags, DTF_DASH | DTF_SLASH), 0);
	SetFlag(FLAG_DASH_SEPARATOR, dtFlags, DTF_DASH);
	SetFlag(FLAG_SLASH_SEPARATOR, dtFlags, DTF_SLASH);
	// country
	SetFlag(FLAG_ASIAN_FORMAT, dtFlags, DTF_ASIAN_FORMAT);
	SetFlag(FLAG_EUROPEAN_FORMAT, dtFlags, DTF_EUROPEAN_FORMAT);
	SetFlag(FLAG_MILITARY_FORMAT, dtFlags, DTF_MILITARY_FORMAT);
	SetFlag(FLAG_US_FORMAT, dtFlags, DTF_US_FORMAT);
}

void ZAF_LOCALE_WINDOW::LOC_DATE_DataStore(void)
{
	// Store the object data.
//	GetText(FORMAT, table->data.decimalSeparator);
	GetText(SEPARATOR, table->dateSeparator);
	GetText(DATE_STRING_FORMAT, &table->dateStringFormat);
	GetText(DATE_TIME_STRING_FORMAT, &table->dateTimeStringFormat);

	// Store the default flags.
	// format
	DTF_FLAGS dtFlags = 0;
	GetFlag(FLAG_SHORT_DAY, &dtFlags, DTF_SHORT_DAY);
	GetFlag(FLAG_DAY_OF_WEEK, &dtFlags, DTF_DAY_OF_WEEK);
	GetFlag(FLAG_SHORT_MONTH, &dtFlags, DTF_SHORT_MONTH);
	GetFlag(FLAG_MONTH, &dtFlags, DTF_ALPHA_MONTH);
	GetFlag(FLAG_SHORT_YEAR, &dtFlags, DTF_SHORT_YEAR);
	GetFlag(FLAG_UPPER_CASE, &dtFlags, DTF_UPPER_CASE);
	GetFlag(FLAG_ZERO_FILL, &dtFlags, DTF_ZERO_FILL);
	GetFlag(FLAG_SYSTEM, &dtFlags, DTF_SYSTEM);
	// separators
	GetFlag(FLAG_DASH_SEPARATOR, &dtFlags, DTF_DASH);
	GetFlag(FLAG_SLASH_SEPARATOR, &dtFlags, DTF_SLASH);
	// country
	GetFlag(FLAG_ASIAN_FORMAT, &dtFlags, DTF_ASIAN_FORMAT);
	GetFlag(FLAG_EUROPEAN_FORMAT, &dtFlags, DTF_EUROPEAN_FORMAT);
	GetFlag(FLAG_MILITARY_FORMAT, &dtFlags, DTF_MILITARY_FORMAT);
	GetFlag(FLAG_US_FORMAT, &dtFlags, DTF_US_FORMAT);
	table->defaultDateFlags = dtFlags;
}

