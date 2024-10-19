//	Zinc Interface Library - Z_MONTH.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_ZIL_MONTH
#include "p_i18n.hpp"

// ----- ZIL_MONTH ----------------------------------------------------------

static ZIL_NUMBERID _shortMonth[] =
{
	ZIL_SHMON_1,
	ZIL_SHMON_2,
	ZIL_SHMON_3,
	ZIL_SHMON_4,
	ZIL_SHMON_5,
	ZIL_SHMON_6,
	ZIL_SHMON_7,
	ZIL_SHMON_8,
	ZIL_SHMON_9,
	ZIL_SHMON_10,
	ZIL_SHMON_11,
	ZIL_SHMON_12,
	0
};

static ZIL_NUMBERID _abbreviatedMonth[] =
{
	ZIL_ABMON_1,
	ZIL_ABMON_2,
	ZIL_ABMON_3,
	ZIL_ABMON_4,
	ZIL_ABMON_5,
	ZIL_ABMON_6,
	ZIL_ABMON_7,
	ZIL_ABMON_8,
	ZIL_ABMON_9,
	ZIL_ABMON_10,
	ZIL_ABMON_11,
	ZIL_ABMON_12,
	0
};

static ZIL_NUMBERID _normalMonth[] = 
{
	ZIL_MON_1,
	ZIL_MON_2,
	ZIL_MON_3,
	ZIL_MON_4,
	ZIL_MON_5,
	ZIL_MON_6,
	ZIL_MON_7,
	ZIL_MON_8,
	ZIL_MON_9,
	ZIL_MON_10,
	ZIL_MON_11,
	ZIL_MON_12,
	0
};

void ZAF_LANGUAGE_WINDOW::ZIL_MONTH_DataLoad(void)
{
	// Load the object data.
	SetList(LONG_MONTH_TABLE, _normalMonth);
	SetList(SHORT_MONTH_TABLE, _shortMonth);
	SetList(ABBREVIATED_MONTH_TABLE, _abbreviatedMonth);
}

void ZAF_LANGUAGE_WINDOW::ZIL_MONTH_DataStore(void)
{
	// Store the object data.
	GetList(LONG_MONTH_TABLE, _normalMonth);
	GetList(SHORT_MONTH_TABLE, _shortMonth);
	GetList(ABBREVIATED_MONTH_TABLE, _abbreviatedMonth);
}

