//	Zinc Interface Library - Z_SYS.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_ZAF_MESSAGE_WINDOW
#include "p_i18n.hpp"

// ----- ZAF_MESSAGE_WINDOW -------------------------------------------------

void ZAF_LANGUAGE_WINDOW::ZAF_MESSAGE_WINDOW_DataLoad(void)
{
	// Load the object data.
	SetText(OK_OPTION, ZIL_MSG_OK);
	SetText(YES_OPTION, ZIL_MSG_YES);
	SetText(NO_OPTION, ZIL_MSG_NO);
	SetText(ABORT_OPTION, ZIL_MSG_ABORT);
	SetText(RETRY_OPTION, ZIL_MSG_RETRY);
	SetText(IGNORE_OPTION, ZIL_MSG_IGNORE);
	SetText(CANCEL_OPTION, ZIL_MSG_CANCEL);
	SetText(HELP_OPTION, ZIL_MSG_HELP);
}

void ZAF_LANGUAGE_WINDOW::ZAF_MESSAGE_WINDOW_DataStore(void)
{
	// Store the object data.
	GetText(OK_OPTION, ZIL_MSG_OK);
	GetText(YES_OPTION, ZIL_MSG_YES);
	GetText(NO_OPTION, ZIL_MSG_NO);
	GetText(ABORT_OPTION, ZIL_MSG_ABORT);
	GetText(RETRY_OPTION, ZIL_MSG_RETRY);
	GetText(IGNORE_OPTION, ZIL_MSG_IGNORE);
	GetText(CANCEL_OPTION, ZIL_MSG_CANCEL);
	GetText(HELP_OPTION, ZIL_MSG_HELP);
}

