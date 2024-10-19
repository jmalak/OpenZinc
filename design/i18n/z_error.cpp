//	Zinc Interface Library - Z_ERROR.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_UI_ERROR_SYSTEM
#include "p_i18n.hpp"

// ----- UI_ERROR_SYSTEM ----------------------------------------------------

void ZAF_LANGUAGE_WINDOW::UI_ERROR_SYSTEM_DataLoad(void)
{
	// Load the object data.
	SetText(TITLE, MSG_TITLE);
	SetText(OK_SELECTION, MSG_OK);
	SetText(CANCEL_SELECTION, MSG_CANCEL);
	SetText(ICONNAME, MSG_ICONNAME);
	SetText(MOVE_OPTION, MSG_MOVE);
	SetText(CLOSE_OPTION, MSG_CLOSE);
}

void ZAF_LANGUAGE_WINDOW::UI_ERROR_SYSTEM_DataStore(void)
{
	// Store the object data.
	GetText(TITLE, MSG_TITLE);
	GetText(OK_SELECTION, MSG_OK);
	GetText(CANCEL_SELECTION, MSG_CANCEL);
	GetText(ICONNAME, MSG_ICONNAME);
	GetText(MOVE_OPTION, MSG_MOVE);
	GetText(CLOSE_OPTION, MSG_CLOSE);
}

