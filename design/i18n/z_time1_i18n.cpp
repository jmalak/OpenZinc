//	Zinc Interface Library - Z_TIME.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_UIW_TIME
#include "p_i18n.hpp"

// ----- UIW_TIME -----------------------------------------------------------

void ZAF_LANGUAGE_WINDOW::UIW_TIME_DataLoad(void)
{
	// Load the object data.
	SetText(INVALID, TMI_INVALID);
	SetText(VALUE_MISSING, TMI_VALUE_MISSING);
	SetText(OUT_OF_RANGE, TMI_OUT_OF_RANGE);
	SetText(LESS_THAN_RANGE, TMI_LESS_THAN_RANGE);
	SetText(GREATER_THAN_RANGE, TMI_GREATER_THAN_RANGE);
}

void ZAF_LANGUAGE_WINDOW::UIW_TIME_DataStore(void)
{
	// Store the object data.
	GetText(INVALID, TMI_INVALID);
	GetText(VALUE_MISSING, TMI_VALUE_MISSING);
	GetText(OUT_OF_RANGE, TMI_OUT_OF_RANGE);
	GetText(LESS_THAN_RANGE, TMI_LESS_THAN_RANGE);
	GetText(GREATER_THAN_RANGE, TMI_GREATER_THAN_RANGE);
}

