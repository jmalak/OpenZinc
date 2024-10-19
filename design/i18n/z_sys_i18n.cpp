//	Zinc Interface Library - Z_SYS.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_UIW_SYSTEM_BUTTON
#include "p_i18n.hpp"

// ----- UIW_SYSTEM_BUTTON --------------------------------------------------

void ZAF_LANGUAGE_WINDOW::UIW_SYSTEM_BUTTON_DataLoad(void)
{
	// Load the object data.
	SetText(RESTORE_OPTION, MNIF_RESTORE);
	SetText(MOVE_OPTION, MNIF_MOVE);
	SetText(SIZE_OPTION, MNIF_SIZE);
	SetText(MINIMIZE_OPTION, MNIF_MINIMIZE);
	SetText(MAXIMIZE_OPTION, MNIF_MAXIMIZE);
	SetText(CLOSE_OPTION, MNIF_CLOSE);
}

void ZAF_LANGUAGE_WINDOW::UIW_SYSTEM_BUTTON_DataStore(void)
{
	// Store the object data.
	GetText(RESTORE_OPTION, MNIF_RESTORE);
	GetText(MOVE_OPTION, MNIF_MOVE);
	GetText(SIZE_OPTION, MNIF_SIZE);
	GetText(MINIMIZE_OPTION, MNIF_MINIMIZE);
	GetText(MAXIMIZE_OPTION, MNIF_MAXIMIZE);
	GetText(CLOSE_OPTION, MNIF_CLOSE);
}

