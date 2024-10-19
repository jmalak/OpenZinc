//	Zinc Interface Library - Z_TIME.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_ZIL_TIME
#include "p_i18n.hpp"

// ----- ZIL_TIME -----------------------------------------------------------

void ZAF_LANGUAGE_WINDOW::ZIL_TIME_DataLoad(void)
{
	// Load the object data.
	SetText(AM_PTR, ZIL_AM);
	SetText(PM_PTR, ZIL_PM);
	SetText(TIME_ZONE, ZIL_TZ);
}

void ZAF_LANGUAGE_WINDOW::ZIL_TIME_DataStore(void)
{
	// Store the object data.
	GetText(AM_PTR, ZIL_AM);
	GetText(PM_PTR, ZIL_PM);
	GetText(TIME_ZONE, ZIL_TZ);
}

