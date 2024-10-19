//	Zinc Interface Library - Z_DAY.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_ZIL_DAY
#include "p_i18n.hpp"

static ZIL_NUMBERID _shortDay[] =
{
	ZIL_SHDAY_1,
	ZIL_SHDAY_2,
	ZIL_SHDAY_3,
	ZIL_SHDAY_4,
	ZIL_SHDAY_5,
	ZIL_SHDAY_6,
	ZIL_SHDAY_7,
	0
};

static ZIL_NUMBERID _abbreviatedDay[] =
{
	ZIL_ABDAY_1,
	ZIL_ABDAY_2,
	ZIL_ABDAY_3,
	ZIL_ABDAY_4,
	ZIL_ABDAY_5,
	ZIL_ABDAY_6,
	ZIL_ABDAY_7,
	0
};

static ZIL_NUMBERID _normalDay[] = 
{
	ZIL_DAY_1,
	ZIL_DAY_2,
	ZIL_DAY_3,
	ZIL_DAY_4,
	ZIL_DAY_5,
	ZIL_DAY_6,
	ZIL_DAY_7,
	0
};

// ----- ZIL_DAY ------------------------------------------------------------

void ZAF_LANGUAGE_WINDOW::ZIL_DAY_DataLoad(void)
{
	// Load the object data.
	SetList(LONG_DAY_TABLE, _normalDay);
	SetList(SHORT_DAY_TABLE, _shortDay);
	SetList(ABBREVIATED_DAY_TABLE, _abbreviatedDay);
}

void ZAF_LANGUAGE_WINDOW::ZIL_DAY_DataStore(void)
{
	// Store the object data.
	GetList(LONG_DAY_TABLE, _normalDay);
	GetList(SHORT_DAY_TABLE, _shortDay);
	GetList(ABBREVIATED_DAY_TABLE, _abbreviatedDay);
}

