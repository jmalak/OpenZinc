//	Zinc Interface Library - Z_WINDOW.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_UIW_WINDOW
#include "p_i18n.hpp"

// ----- UIW_WINDOW ---------------------------------------------------------

void ZAF_LANGUAGE_WINDOW::UIW_WINDOW_DataLoad(void)
{
	// Load the object data.
	SetText(OBJECT_ERROR, -1);
	SetText(UNKNOWN_OBJECT, ZIL_UNK_OBJ);
	SetText(OBJECT_NOT_FOUND, ZIL_NO_OBJ);
	SetText(FILE_NOT_FOUND, ZIL_NO_FILE);
}

void ZAF_LANGUAGE_WINDOW::UIW_WINDOW_DataStore(void)
{
	// Store the object data.
	GetText(OBJECT_ERROR, -1);
	GetText(UNKNOWN_OBJECT, ZIL_UNK_OBJ);
	GetText(OBJECT_NOT_FOUND, ZIL_NO_OBJ);
	GetText(FILE_NOT_FOUND, ZIL_NO_FILE);
}

