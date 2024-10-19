//	Zinc Interface Library - Z_DATE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_UIW_DATE
#include "p_i18n.hpp"

// ----- UIW_DATE -----------------------------------------------------------

void ZAF_LANGUAGE_WINDOW::UIW_DATE_DataLoad(void)
{
	// Load the object data.
	SetText(INVALID, DTI_INVALID);
	SetText(AMBIGUOUS, DTI_AMBIGUOUS);
	SetText(INVALID_NAME, DTI_INVALID_NAME);
	SetText(VALUE_MISSING, DTI_VALUE_MISSING);
	SetText(OUT_OF_RANGE, DTI_OUT_OF_RANGE);
	SetText(LESS_THAN_RANGE, DTI_LESS_THAN_RANGE);
	SetText(GREATER_THAN_RANGE, DTI_GREATER_THAN_RANGE);
}

void ZAF_LANGUAGE_WINDOW::UIW_DATE_DataStore(void)
{
	// Store the object data.
	GetText(INVALID, DTI_INVALID);
	GetText(AMBIGUOUS, DTI_AMBIGUOUS);
	GetText(INVALID_NAME, DTI_INVALID_NAME);
	GetText(VALUE_MISSING, DTI_VALUE_MISSING);
	GetText(OUT_OF_RANGE, DTI_OUT_OF_RANGE);
	GetText(LESS_THAN_RANGE, DTI_LESS_THAN_RANGE);
	GetText(GREATER_THAN_RANGE, DTI_GREATER_THAN_RANGE);
}

