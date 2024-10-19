//	Zinc Interface Library - Z_INTEGER.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_UIW_INTEGER
#include "p_i18n.hpp"

// ----- UIW_INTEGER --------------------------------------------------------

void ZAF_LANGUAGE_WINDOW::UIW_INTEGER_DataLoad(void)
{
	// Load the object data.
	SetText(INVALID, NMI_INVALID);
	SetText(OUT_OF_RANGE, NMI_OUT_OF_RANGE);
	SetText(LESS_THAN_RANGE, NMI_LESS_THAN_RANGE);
	SetText(GREATER_THAN_RANGE, NMI_GREATER_THAN_RANGE);
}

void ZAF_LANGUAGE_WINDOW::UIW_INTEGER_DataStore(void)
{
	// Store the object data.
	GetText(INVALID, NMI_INVALID);
	GetText(OUT_OF_RANGE, NMI_OUT_OF_RANGE);
	GetText(LESS_THAN_RANGE, NMI_LESS_THAN_RANGE);
	GetText(GREATER_THAN_RANGE, NMI_GREATER_THAN_RANGE);
}

